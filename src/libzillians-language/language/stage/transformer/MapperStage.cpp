/**
 * Zillians MMO
 * Copyright (C) 2007-2011 Zillians.com, Inc.
 * For more information see http://www.zillians.com
 *
 * Zillians MMO is the library and runtime for massive multiplayer online game
 * development in utility computing model, which runs as a service for every
 * developer to build their virtual world running on our GPU-assisted machines.
 *
 * This is a close source library intended to be used solely within Zillians.com
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "language/stage/transformer/MapperStage.h"
#include "language/context/ParserContext.h"
#include "language/stage/transformer/visitor/MapperStageVisitor.h"
#include "language/tree/visitor/NameManglingVisitor.h"
#include "language/stage/serialization/detail/ASTSerializationHelper.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::NameManglingVisitor;
using zillians::language::stage::visitor::MapperStageVisitor;

namespace zillians { namespace language { namespace stage {

namespace detail {

using namespace zillians::language::tree;

static bool isImplicitVariableDeclaration(VariableDecl* decl)
{
	ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(decl->type);
	if(!resolved_type) return false;

	if(isa<ClassDecl>(resolved_type))
	{
		ClassDecl* class_decl = cast<ClassDecl>(resolved_type);
		if(!ASTNodeHelper::findAnnotation(class_decl, L"implicit"))
			return false;
	}

	return true;
}

static TypeSpecifier* getTypeSpecifierForImplicitVariable(VariableDecl* decl)
{
	ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(decl->type);
	if(!resolved_type) return false;

	if(isa<ClassDecl>(resolved_type))
	{
		ClassDecl* class_decl = cast<ClassDecl>(resolved_type);
		foreach(i, class_decl->member_variables)
		{
			if((*i)->name->toString() == L"value")
			{
				return ASTNodeHelper::createTypeSpecifierFrom((*i)->type);
			}
		}
	}

	return NULL;
}

struct ImplicitVariableParameterIndex
{
	explicit ImplicitVariableParameterIndex(int i) : index(i) { }
	int index;
};

}

MapperStage::MapperStage() : enabled(false)
{ }

MapperStage::~MapperStage()
{ }

const char* MapperStage::name()
{
	return "Mapper Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> MapperStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("mapper-ast", po::value<std::vector<std::string>>(), "AST file for mapper");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool MapperStage::parseOptions(po::variables_map& vm)
{
	if(vm.count("mapper-ast") > 0)
	{
		enabled = true;
		mapper_ast_to_load = vm["mapper-ast"].as<std::vector<std::string>>();
	}

	return true;
}

bool MapperStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

	if(!hasParserContext())
		return false;

	ParserContext& parser_context = getParserContext();

	NameManglingVisitor mangler;
	MapperStageVisitor visitor;

	if(parser_context.tangle)
	{
		if(enabled)
		{
			// load all mapper ASTs
			foreach(i, mapper_ast_to_load)
			{
				ASTNode* ast = ASTSerializationHelper::deserialize(*i);
				mapper_ast.push_back(ast);
			}

			// TODO create the final form of the mapped system call (convert all implicit variable to actual type and variable declaration)
			// TODO the index of the appended implicit variable will be saved as a temporary context attached to the implicit variable declaration (and will be cleaned up later on)

			// collect all implicit variables which will be used to append to all user functions
			foreach(i, mapper_ast)
			{
				ASTNodeHelper::foreachApply<ASTNode>(**i, [&](ASTNode& node) {
					if(isa<ClassDecl>(&node) && ASTNodeHelper::findAnnotation(&node, L"implicit"))
					{
						ClassDecl* class_decl = cast<ClassDecl>(&node);
						implicit_variables.push_back(class_decl);

						class_decl->set<detail::ImplicitVariableParameterIndex>(
								new detail::ImplicitVariableParameterIndex(
										implicit_variables.size() - 1));
					}
				});
			}

			// create hash table for system functions (using mangled name as key)
			foreach(i, mapper_ast)
			{
				ASTNodeHelper::foreachApply<ASTNode>(**i, [&](ASTNode& node) {
					if(isa<FunctionDecl>(&node) && ASTNodeHelper::findAnnotation(&node, L"system"))
					{
						FunctionDecl* function_decl = cast<FunctionDecl>(&node);

						mangler.visit(node);
						const std::string& mangled_name = mangler.mOutStream.str();
						hashed_system_functions[mangled_name] = function_decl;
						mangler.reset();
					}
				});
			}

			// TODO gcc 4.5.2 does not handle function type implicit conversion from lambda to std::function correctly, so we have to declare a type explicitly
			// TODO and pass it to MapperStageVisitor::addMapper()

			// TODO we need a way to figure out the caller scope, either from global dispatch or from user function
			// TODO so obviously the mapper stage should happen "after" generating global dispatch

			// rewrite all callee signature for calling user function
			MapperStageVisitor::FunctionDeclMapper user_callee_rewrite_mapper = [&](FunctionDecl& node, std::vector<std::function<void()>>& transform) {
				if(isa<FunctionDecl>(&node) && !ASTNodeHelper::findAnnotation(&node, L"system") && !ASTNodeHelper::findAnnotation(&node, L"native"))
				{
					std::vector<VariableDecl*> prepend_parameters;
					foreach(i, implicit_variables)
					{
						VariableDecl* v = new VariableDecl(
								new SimpleIdentifier(L"$"),
								ASTNodeHelper::createTypeSpecifierFrom(*i),
								false, false, false, Declaration::VisibilitySpecifier::DEFAULT);
						prepend_parameters.push_back(v);
					}

					node.parameters.insert(node.parameters.begin(), prepend_parameters.begin(), prepend_parameters.end());
				}
			};
			visitor.addMapper(user_callee_rewrite_mapper);

			// rewrite all callee signature for system function
			MapperStageVisitor::FunctionDeclMapper system_callee_rewrite_mapper = [&](FunctionDecl& node, std::vector<std::function<void()>>& transform) {
				// TODO replace the system function signature with the one defined in mapper AST
				if(isa<FunctionDecl>(&node) && ASTNodeHelper::findAnnotation(&node, L"system"))
				{
					mangler.visit(node);
					const std::string& mangled_name = mangler.mOutStream.str();

					// try to find the corresponding system function in the mapper AST
					auto f = hashed_system_functions.find(mangled_name);
					if(f != hashed_system_functions.end())
					{
						FunctionDecl* mapped_function = cast<FunctionDecl>(f->second);

						auto i = node.parameters.begin();
						foreach(j, mapped_function->parameters)
						{
							if(detail::isImplicitVariableDeclaration(*j))
							{
								i = node.parameters.insert(i, cast<VariableDecl>((*j)->clone()));
							}
							++i;
						}
					}

					mangler.reset();
				}
			};
			visitor.addMapper(system_callee_rewrite_mapper);

			// rewrite all caller arguments for user function
			MapperStageVisitor::CallExprMapper user_caller_rewrite_mapper = [&](CallExpr& node, std::vector<std::function<void()>>& transform) {
				FunctionDecl* owner_function = ASTNodeHelper::getOwner<FunctionDecl>(&node);

				if(ASTNodeHelper::hasAnnotation(owner_function, L"global"))
				{
					// TODO if the calling site is the global dispatch, we need different handling here
				}
				else
				{
					// if the calling site is an user function, we just need to forward the arguments
					std::vector<Expression*> prepend_arguments;
					int j = 0;
					foreach(i, implicit_variables)
					{
						PrimaryExpr* arg = new PrimaryExpr(new SimpleIdentifier(L"$"));
						ResolvedSymbol::set(arg, owner_function->parameters[j]);
						prepend_arguments.push_back(arg);
						++j;
					}

					node.parameters.insert(node.parameters.begin(), prepend_arguments.begin(), prepend_arguments.end());
				}
			};
			visitor.addMapper(user_caller_rewrite_mapper);

			// rewrite all caller arguments for calling system function
			MapperStageVisitor::CallExprMapper system_caller_rewrite_mapper = [&](CallExpr& node, std::vector<std::function<void()>>& transform) {
				ASTNode* resolved_function = ASTNodeHelper::findUniqueSymbolResolution(&node);
				if(resolved_function)
				{
					if(isa<FunctionDecl>(resolved_function) && ASTNodeHelper::findAnnotation(resolved_function, L"system"))
					{
						mangler.visit(node);
						const std::string& mangled_name = mangler.mOutStream.str();

						// try to find the corresponding system function in the mapper AST
						auto f = hashed_system_functions.find(mangled_name);
						if(f != hashed_system_functions.end())
						{
							FunctionDecl* mapped_function = cast<FunctionDecl>(f->second);

							// if found, we have to iterate all parameters of that function and insert some addition argument to the caller
							foreach(j, mapped_function->parameters)
							{
								if(detail::isImplicitVariableDeclaration(*j))
								{
									TypeSpecifier* type_specifier_to_replace = detail::getTypeSpecifierForImplicitVariable(*j);
								}
							}

							// TODO how to get those variables?
						}

						mangler.reset();
					}
				}
			};
			visitor.addMapper(system_caller_rewrite_mapper);

			// visit all nodes in the user's AST
			visitor.visit(*parser_context.tangle);

			// apply transform if there's any
			if(visitor.hasTransforms())
				visitor.applyTransforms();
		}

		return true;
	}
	else
	{
		return false;
	}
}

} } }
