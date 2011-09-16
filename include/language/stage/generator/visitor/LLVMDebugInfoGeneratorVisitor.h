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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORVISITOR_H_

#include "core/Prerequisite.h"
#include <boost/filesystem.hpp>

#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/context/ParserContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/stage/generator/context/DebugInfoContext.h"
#include "language/stage/generator/detail/LLVMForeach.h"
#include "utility/UnicodeUtil.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians {
namespace language {
namespace stage {
namespace visitor {

#define COMPANY_INFORMATION "1.0 ThorScript Compiler (Zillians Corp.)"


struct LLVMDebugInfoGeneratorVisitor: GenericDoubleVisitor
{
	CREATE_INVOKER(generateInvoker, generate)

	typedef std::map<PrimitiveType::type, llvm::DIType> type_cache_t;

	LLVMDebugInfoGeneratorVisitor(llvm::LLVMContext& context, llvm::Module& current_module) :
		context(context), current_module(current_module), factory(current_module)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	void generate(ASTNode& node)
	{
		/**
		 * Generic implementation:
		 *
		 * 1. Propagate the debug info from parent node to child node
		 * 2. Insert default debug information (line, column, context)
		 *
		 * Usually, this function is good for the leaf nodes. If the node is not leaf, we need to specify the context manually, like function, block.
		 */
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);

		if (node.parent)
		{
			// By pass the debug information from parent to the child.
			DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);

			// Only Program and Package has no debug info context. So we could safely skip them
			if (parent_debug_info)
			{
				// Propagate the debug info to the current node
				DebugInfoContext::set(&node, new DebugInfoContext(*parent_debug_info));

				SourceInfoContext* source_info = SourceInfoContext::get(&node);
				llvm::Value* llvm_value = node.get<llvm::Value>();

				if (llvm_value)
				{
					llvm::Instruction* llvm_inst = llvm::dyn_cast<llvm::Instruction>(llvm_value);
					if (llvm_inst)
						llvm_inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, parent_debug_info->context));
				}

			}
		}

		revisit(node);
	}

	void generate(Program& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		ModuleSourceInfoContext* module_info = ModuleSourceInfoContext::get(&node);

		DebugInfoProgramContext *program_context = new DebugInfoProgramContext();

		// Create compile units
		for (int i = 0; i < module_info->source_files.size(); i++)
		{
			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> Ori path: " << module_info->source_files[i]);
			boost::filesystem::path file_path(module_info->source_files[i]);
			std::string folder = file_path.parent_path().generic_string();
			std::string filename = file_path.filename().generic_string();

			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> folder: " << folder);
			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> filename: " << filename);

			factory.createCompileUnit(llvm::dwarf::DW_LANG_C_plus_plus,
				llvm::StringRef(filename.c_str()), llvm::StringRef(folder.c_str()), llvm::StringRef(COMPANY_INFORMATION),
				/*optimized*/false, /*flags*/llvm::StringRef(""), /*runtime version*/0);

			llvm::DIFile file = factory.createFile(llvm::StringRef(filename.c_str()), llvm::StringRef(folder.c_str()));

			llvm::DICompileUnit compile_unit(factory.getCU());
			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> compile_unit: " << compile_unit);
			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> file: " << file);

			// The index of the compile_units corresponds to the index of module_info->source_files
			program_context->addProgramContext(compile_unit, file);
		}

		DebugInfoProgramContext::set(&node, program_context);

		revisit(node);
	}

	void generate(TypeSpecifier& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);
		DebugInfoProgramContext* program_context = DebugInfoProgramContext::get(getParserContext().program);

		int32 source_index = source_info->source_index;

		llvm::DIType type;
		switch(node.type)
		{
		case TypeSpecifier::ReferredType::CLASS_DECL: break;
		case TypeSpecifier::ReferredType::FUNCTION_DECL:  break;
		case TypeSpecifier::ReferredType::ENUM_DECL: break;
		case TypeSpecifier::ReferredType::TYPEDEF_DECL: break;
		case TypeSpecifier::ReferredType::FUNCTION_TYPE: break;
		case TypeSpecifier::ReferredType::UNSPECIFIED: break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
		{
			type = createPrimitiveType(
					primitive_type_caches[source_index],
					node.referred.primitive);
			break;
		}
		}

		DebugInfoTypeContext::set(&node, new DebugInfoTypeContext(type));
	}

	void generate(FunctionDecl& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Function> function name: " << ws_to_s(node.name->toString()));

		SourceInfoContext* source_info = SourceInfoContext::get(&node);
		NameManglingContext* mangling = NameManglingContext::get(&node);

		int32 source_index = source_info->source_index;
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage,"<Function> mangling name: " << mangling->managled_name);

		DebugInfoProgramContext* program_context = DebugInfoProgramContext::get(getParserContext().program);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Function> file: " << program_context->files[source_index]);

		// Generate return type debug information
		generate(*node.type);
		DebugInfoTypeContext* return_type = DebugInfoTypeContext::get(node.type);

		// TODO: Generate debug information of parameters' type

		// Create DISubprogram for the function
		llvm::Function * llvm_function = node.get<llvm::Function>();
		llvm::DISubprogram subprogram = factory.createFunction(
				program_context->files[source_index], // TODO: Last context
				llvm::StringRef(ws_to_s(node.name->toString()).c_str()),
				llvm::StringRef(mangling->managled_name.c_str()),
				program_context->files[source_index],
				source_info->line,
				return_type->type,
				false, //bool isLocalToUnit,
				true, //bool isDefinition,
				false, // unsigned flags: is this function prototyped or not
				false, //bool isOptimized = false
				llvm_function);

		DebugInfoContext::set(&node, new DebugInfoContext(program_context->compile_units[source_index],
				program_context->files[source_index], subprogram));

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Function> subprogram: " << subprogram << " mdnode: " << (llvm::MDNode*)subprogram);

		// Visit other attributes
		if(node.name) generate(*node.name);
		foreach(i, node.parameters)
		{
			if(i->get<0>()) generate(*i->get<0>());
			if(i->get<1>()) generate(*i->get<1>());
			if(i->get<2>()) generate(*i->get<2>());
		}
		if(node.block) generate(*node.block);
		if(node.annotations) generate(*node.annotations);
	}

	void generate(Block& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		BOOST_ASSERT(node.parent && "Block has no parent!");

		// Retrieve parent node debug information, since we need its context
		DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Block> parent: " << parent_debug_info->context << " mdnode: " << (llvm::MDNode*)parent_debug_info->context);
		llvm::DILexicalBlock function_block = factory.createLexicalBlock(
				parent_debug_info->context, parent_debug_info->file, source_info->line, source_info->column);

		DebugInfoContext::set(&node, new DebugInfoContext(
				parent_debug_info->compile_unit, parent_debug_info->file,	// inherit from parent node
				function_block));
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Block> context: " << function_block);
		revisit(node);
	}

	void generate(VariableDecl& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		BOOST_ASSERT(node.parent && "Variable declaration has no parent!");

		DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> parent context: " << parent_debug_info->context);

		// Generate type debug information
		generate(*node.type);
		DebugInfoTypeContext* type_info = DebugInfoTypeContext::get(node.type);

		// TODO: Need to decide the type
		llvm::DIVariable variable = factory.createLocalVariable(
				llvm::dwarf::DW_TAG_auto_variable,
				parent_debug_info->context, llvm::StringRef(ws_to_s(node.name->toString()).c_str()),
				parent_debug_info->file, source_info->line,
				type_info->type
				);

		DebugInfoContext::set(&node, new DebugInfoContext(
				parent_debug_info->compile_unit, parent_debug_info->file, // inherit from parent node
				variable));

		// TODO:: not sure the llvm value and llvm instruction
		llvm::Value* value = node.get<llvm::Value>();
		llvm::BasicBlock* block = llvm::cast<llvm::Instruction>(value)->getParent();

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> value: " << value);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> block: " << block);

		// TODO: decide to insert at end of block or before an specific instruction
		llvm::Instruction* variable_inst = factory.insertDeclare(value, variable, block);
		llvm::MDNode* scope = parent_debug_info->context;
		variable_inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, scope));

		// Check if the variable has initialization
		llvm::StoreInst* store_inst = node.get<llvm::StoreInst>();
		if (store_inst)
		{
			store_inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, scope));
		}

		revisit(node);
	}

	void generate(IfElseStmt& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);

		DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);
		DebugInfoContext::set(&node, new DebugInfoContext(*parent_debug_info));

		SourceInfoContext* source_info = SourceInfoContext::get(&node);

		revisit(node);

		llvm::Instruction* inst = llvm::cast<llvm::Instruction>(node.if_branch.cond->get<llvm::Value>());
		inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, parent_debug_info->context));
	}


private:
	llvm::DIType createPrimitiveType(type_cache_t& primitive_type_cache, PrimitiveType::type type)
	{
		int bits = 0;
		int alignment = 0;
		llvm::dwarf::dwarf_constants encoding;

		if (primitive_type_cache.find(type) != primitive_type_cache.end())
		{
			return primitive_type_cache[type];
		}

		switch (type)
		{
		case PrimitiveType::VOID: break;
		case PrimitiveType::INT8: break;
		case PrimitiveType::INT16: break;
		case PrimitiveType::INT32: break;
		case PrimitiveType::INT64: break;
		case PrimitiveType::UINT8: break;
		case PrimitiveType::UINT16: break;
		case PrimitiveType::UINT32:
		{
			bits = 32; alignment = 32;
			encoding = llvm::dwarf::DW_ATE_unsigned;
			break;
		}
		case PrimitiveType::UINT64:
		{
			bits = 64; alignment = 64;
			encoding = llvm::dwarf::DW_ATE_unsigned;
			break;
		}
		case PrimitiveType::FLOAT32: break;
		case PrimitiveType::FLOAT64: break;
		case PrimitiveType::ANONYMOUS_OBJECT: break;
		case PrimitiveType::ANONYMOUS_FUNCTION: break;
		case PrimitiveType::VARIADIC_ELLIPSIS: break;
		default:
			BOOST_ASSERT(false && "Unknown basic type");
		}

		llvm::DIType basic_type = factory.createBasicType(
				llvm::StringRef(ws_to_s(PrimitiveType::toString(type)).c_str()),
				bits, alignment, encoding);

		return (primitive_type_cache[type] = basic_type);
	}

private:
	llvm::LLVMContext& context;
	llvm::Module& current_module;

	llvm::DIBuilder factory;

	// The first key is the source index, each source
	std::map< unsigned int, type_cache_t > primitive_type_caches;
};

}}}}

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORVISITOR_H_ */
