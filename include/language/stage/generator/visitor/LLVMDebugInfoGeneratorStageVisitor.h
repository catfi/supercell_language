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

#ifndef ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORSTAGEVISITOR_H_
#define ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORSTAGEVISITOR_H_

#include "core/Prerequisite.h"
#include <boost/filesystem.hpp>

#include "utility/Filesystem.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "language/context/ParserContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/stage/generator/context/DebugInfoContext.h"
#include "language/stage/generator/context/SynthesizedFunctionContext.h"
#include "language/stage/generator/context/SynthesizedValueContext.h"
#include "language/stage/generator/detail/LLVMForeach.h"
#include "utility/UnicodeUtil.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

#define COMPANY_INFORMATION "1.0 ThorScript Compiler (Zillians Inc)"

struct LLVMDebugInfoGeneratorStageVisitor: public GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

	typedef std::map<PrimitiveType::type, llvm::DIType> type_cache_t;

	LLVMDebugInfoGeneratorStageVisitor(llvm::LLVMContext& context, llvm::Module& current_module) :
		context(context), current_module(current_module), factory(current_module)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	~LLVMDebugInfoGeneratorStageVisitor()
	{
		// Call this function to generated all deferred debug information
		factory.finalize();
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
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "Handle General Node");
		insertDebugLocation(node);
		insertDebugLocationForIntermediateValues(node);
		revisit(node);
	}

	void generate(CallExpr& node)
	{
		insertDebugLocation(node);
	}

	void generate(Source& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);

		// Create compile units
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> Ori path: " << node.filename);
		boost::filesystem::path absolute_filepath = Filesystem::normalize_path(node.filename);
		std::string absolute_folder = absolute_filepath.parent_path().generic_string();
		std::string absolute_filename = absolute_filepath.generic_string();
		std::string filename = absolute_filepath.filename().generic_string();

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> absolute folder: " << absolute_folder);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> absolute filename: " << absolute_filename);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> filename: " << filename);

		factory.createCompileUnit(llvm::dwarf::DW_LANG_C_plus_plus,
			llvm::StringRef(absolute_filename.c_str()), llvm::StringRef(absolute_folder.c_str()), llvm::StringRef(COMPANY_INFORMATION),
			/*optimized*/false, /*flags*/llvm::StringRef(""), /*runtime version*/0);

		llvm::DIFile file = factory.createFile(llvm::StringRef(filename.c_str()), llvm::StringRef(absolute_folder.c_str()));

		llvm::DICompileUnit compile_unit(factory.getCU());
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> compile_unit: " << compile_unit);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Program> file: " << file);

		DebugInfoContext::set(&node, new DebugInfoContext(compile_unit, file, file));

		revisit(node);
	}

	void generate(TypeSpecifier& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);
		//DebugInfoProgramContext* program_context = DebugInfoProgramContext::get(getParserContext().program);

		llvm::DIType type;
		switch(node.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE: break;
		case TypeSpecifier::ReferredType::UNSPECIFIED: break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
		{
			type = createPrimitiveType(node.referred.primitive);
			DebugInfoTypeContext::set(&node, new DebugInfoTypeContext(type));
			break;
		}
		}
	}

	void generate(FunctionDecl& node)
	{
		if(ASTNodeHelper::hasNativeLinkage(&node))
			return;

		NameManglingContext* mangling = NameManglingContext::get(&node);
		DebugInfoContext* file_context = DebugInfoContext::get(getParserContext().active_source);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage,"<Function> (" << getParserContext().active_source->filename << ") " << ws_to_s(node.name->toString()) << ", " << mangling->managled_name);

		// generate return type debug information
		generate(*node.type);
		llvm::DIType subroutine_type = createSubroutineType(node, file_context->file);

		// Create DISubprogram for the function
		llvm::Function* llvm_function = GET_SYNTHESIZED_LLVM_FUNCTION(&node);
		BOOST_ASSERT(llvm_function && "Well, the llvm function is NULL");

		llvm::DISubprogram subprogram = factory.createFunction(
				DebugInfoContext::get(node.parent)->context,
				llvm::StringRef(ws_to_s(node.name->toString()).c_str()),
				llvm::StringRef(mangling->managled_name.c_str()),
				file_context->file,
				SourceInfoContext::get(&node)->line,
				subroutine_type,
				false, //bool isLocalToUnit,
				true, //bool isDefinition,
				llvm::DIDescriptor::FlagPrototyped, // TODO: Decide the flag
				false, //bool isOptimized = false
				llvm_function);

		DebugInfoContext::set(&node, new DebugInfoContext(file_context->compile_unit, file_context->file, subprogram));
//		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Function> subprogram: " << subprogram << " mdnode: " << (llvm::MDNode*)subprogram);

		// Generate block and parameters debug info
		for (int i = 0; i < node.parameters.size(); i++)
		{
			// generate debug information of parameters' type
			generateVariableDebugInfo(*node.parameters[i], llvm::dwarf::DW_TAG_arg_variable, i+1);
		}
		if(node.block) generate(*node.block);

		// Generate function epilog
		createFunctionEpilog(node);
	}

	void generate(Block& node)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		BOOST_ASSERT(node.parent && "Block has no parent!");

		// Retrieve parent node debug information, since we need its context
		DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Block> line: " << source_info->line << " column: " << source_info->column);
		llvm::DILexicalBlock function_block = factory.createLexicalBlock(
				parent_debug_info->context, parent_debug_info->file, source_info->line, source_info->column);

		DebugInfoContext::set(&node, new DebugInfoContext(
				parent_debug_info->compile_unit, parent_debug_info->file,	// inherit from parent node
				function_block));
		revisit(node);
	}

	void generate(VariableDecl& node)
	{
		generateVariableDebugInfo(node, llvm::dwarf::DW_TAG_auto_variable);
	}

private:
	llvm::DIType createSubroutineType(FunctionDecl& node, llvm::DIFile file)
	{
		DebugInfoTypeContext* return_type = DebugInfoTypeContext::get(node.type);

		// Create subrountine type
		std::vector<llvm::Value*> elements;
		elements.push_back(return_type->type);

		// TODO: By observing clang, I found that even I give lots of parameters, it only generates return type. Parameters seem to lose.
		// So, temporarily we only put the return type.
//		foreach(i, node.parameters)
//		{
//			elements.push_back( GET_SYNTHESIZED_LLVM_FUNCTION(*i) );
//		}
		llvm::DIArray parameter_types = factory.getOrCreateArray(llvm::ArrayRef<llvm::Value*>(elements));
		return factory.createSubroutineType(file, parameter_types);
	}

	void createFunctionEpilog(FunctionDecl& node)
	{
		llvm::Function* llvm_function = GET_SYNTHESIZED_LLVM_FUNCTION(&node);

		auto last_block = llvm_function->getBasicBlockList().rbegin();
		llvm::TerminatorInst* terminate_inst = last_block->getTerminator();
		DebugInfoContext* block_debug_info = DebugInfoContext::get(node.block);
		SourceInfoContext* block_source_info = SourceInfoContext::get(node.block);
		BOOST_ASSERT(block_debug_info && block_source_info && "No debug info or source info for block");
		terminate_inst->setDebugLoc(llvm::DebugLoc::get(block_source_info->line, block_source_info->column, block_debug_info->context));
	}

	void generateVariableDebugInfo(VariableDecl& node, llvm::dwarf::llvm_dwarf_constants variable_type, int argument_position = 0)
	{
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __PRETTY_FUNCTION__);
		BOOST_ASSERT(node.parent && "Variable declaration has no parent!");

		DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> parent context: " << parent_debug_info->context);

		// generate type debug information
		generate(*node.type);
		DebugInfoTypeContext* type_info = DebugInfoTypeContext::get(node.type);

		// Well, it should be "this", or there will be some other issue
		if (type_info == NULL)
		{
			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> var: " << ws_to_s(node.name->toString()).c_str() << " has no type info");
			return;
		}

		// TODO: Need to decide the type
		BOOST_ASSERT( (source_info->line >> 24) == 0 && "To much lines!! Only allowed 16777215 lines");
		uint32 offset = (argument_position << 24) + source_info->line;
		llvm::DIVariable variable = factory.createLocalVariable(
				variable_type,
				parent_debug_info->context, llvm::StringRef(ws_to_s(node.name->toString()).c_str()),
				parent_debug_info->file, offset,
				type_info->type
				);

		DebugInfoContext::set(&node, new DebugInfoContext(
				parent_debug_info->compile_unit, parent_debug_info->file, // inherit from parent node
				variable));

		// TODO:: not sure the llvm value and llvm instruction
		llvm::Value* value = GET_SYNTHESIZED_LLVM_VALUE(&node);
		llvm::BasicBlock* block = llvm::cast<llvm::Instruction>(value)->getParent();

		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> value: " << value);
		LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, "<Variable> block: " << block);

		// TODO: decide to insert at end of block or before an specific instruction
		llvm::Instruction* variable_inst = factory.insertDeclare(value, variable, block);
		llvm::MDNode* scope = parent_debug_info->context;
		variable_inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, scope));

		// Check if the variable has initialization
		insertDebugLocationForIntermediateValues(node);
	}

	llvm::DIType createPrimitiveType(PrimitiveType::type type)
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
		case PrimitiveType::VOID_TYPE: break;
		case PrimitiveType::INT8_TYPE:
		{
			bits = 8; alignment = 8;
			encoding = llvm::dwarf::DW_ATE_signed;
			break;
		}
		case PrimitiveType::INT16_TYPE:
		{
			bits = 16; alignment = 16;
			encoding = llvm::dwarf::DW_ATE_signed;
			break;
		}
		case PrimitiveType::INT32_TYPE:
		{
			bits = 32; alignment = 32;
			encoding = llvm::dwarf::DW_ATE_signed;
			break;
		}
		case PrimitiveType::INT64_TYPE:
		{
			bits = 64; alignment = 64;
			encoding = llvm::dwarf::DW_ATE_signed;
			break;
		}
		case PrimitiveType::FLOAT32_TYPE: break;
		case PrimitiveType::FLOAT64_TYPE: break;
		case PrimitiveType::OBJECT_TYPE: break;
		case PrimitiveType::FUNCTION_TYPE: break;
		case PrimitiveType::STRING_TYPE: break;
		case PrimitiveType::VARIADIC_ELLIPSIS_TYPE: break;
		default:
			BOOST_ASSERT(false && "Unknown basic type");
			break;
		}

		llvm::DIType basic_type = factory.createBasicType(
				llvm::StringRef(ws_to_s(PrimitiveType::toString(type)).c_str()),
				bits, alignment, encoding);

		return (primitive_type_cache[type] = basic_type);
	}

	void insertDebugLocation(ASTNode& node)
	{
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
				llvm::Value* llvm_value = GET_SYNTHESIZED_LLVM_VALUE(&node);

				if (llvm_value)
				{
					llvm::Instruction* llvm_inst = llvm::dyn_cast<llvm::Instruction>(llvm_value);
					if (llvm_inst)
					{
						LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __FUNCTION__ << ": " << node.instanceName() << " @" << llvm_inst << "(" << source_info->line << ", " << source_info->column << ") with scope: " << (llvm::MDNode*)parent_debug_info->context);
						llvm_inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, parent_debug_info->context));
					}
				}
			}
		}
	}

	void insertDebugLocationForIntermediateValues(ASTNode& node)
	{
		if (node.parent == NULL)
		{
			LOG4CXX_DEBUG(LoggerWrapper::DebugInfoGeneratorStage, __FUNCTION__ << ": " << node.instanceName() << " has no parent");
			BOOST_ASSERT(false);
		}

		unordered_set<llvm::Value*> values = GET_INTERMEDIATE_LLVM_VALUES(&node);
		SourceInfoContext* source_info = SourceInfoContext::get(&node);
		DebugInfoContext* parent_debug_info = DebugInfoContext::get(node.parent);

		foreach(i, values)
		{
			llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(*i);
			inst->setDebugLoc(llvm::DebugLoc::get(source_info->line, source_info->column, parent_debug_info->context));
		}
	}

private:
	llvm::LLVMContext& context;
	llvm::Module& current_module;

	llvm::DIBuilder factory;

	type_cache_t primitive_type_cache;
};

}}}}

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORSTAGEVISITOR_H_ */
