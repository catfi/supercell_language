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
#include "language/stage/generator/detail/LLVMHelper.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "utility/UnicodeUtil.h"

#include "llvm/Support/Dwarf.h"
#include "llvm/Analysis/DebugInfo.h"


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

	// Structure of a compile unit
	struct compile_unit_t
	{
		llvm::DICompileUnit compile_unit;
		llvm::DIFile file;

		// Since we create the DITypes base on DIFile, so type_cache is move into compile unit structure
		std::map<llvm::Type::TypeID, llvm::DIType> type_cache;
	};

	LLVMDebugInfoGeneratorVisitor(llvm::LLVMContext& context, llvm::Module& current_module) :
		context(context), current_module(current_module), factory(current_module), builder(context), helper(context, current_module, builder)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}

	void generate(ASTNode& node)
	{
	}

	void generate(Program& node)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		ModuleSourceInfoContext* module_info = ModuleSourceInfoContext::get(&node);

		// Create compile units
		for (int i = 0; i < module_info->source_files.size(); i++)
		{
			boost::filesystem::path file_path(module_info->source_files[i]);
			std::string folder = file_path.parent_path().generic_string();
			std::string filename = file_path.filename().generic_string();

			llvm::DICompileUnit compile_unit = factory.CreateCompileUnit(llvm::dwarf::DW_LANG_C_plus_plus,
				llvm::StringRef(filename.c_str()), llvm::StringRef(folder.c_str()), llvm::StringRef(COMPANY_INFORMATION), true);

			llvm::DIFile file = factory.CreateFile(llvm::StringRef(filename.c_str()), llvm::StringRef(folder.c_str()), compile_unit);

			// The index of the compile_units corresponds to the index of module_info->source_files
			compile_unit_t unit;
			unit.compile_unit = compile_unit;
			unit.file = file;
			compile_units.push_back(unit);
		}
	}

	void generate(FunctionDecl& node)
	{
	}

	void generate(VariableDecl& node)
	{
		//helper.createAlloca();
	}

	void generate(ExpressionStmt& node)
	{

	}

	void generate(IfElseStmt& node)
	{

	}

	void generate(ForeachStmt& node)
	{

	}

	void generate(WhileStmt& node)
	{

	}

	void generate(SwitchStmt& node)
	{

	}

	void generate(UnaryExpr& node)
	{

	}

	void generate(BinaryExpr& node)
	{

	}

	void generate(TernaryExpr& node)
	{

	}

	void generate(CallExpr& node)
	{

	}

	void generate(CastExpr& node)
	{

	}

	void generate(MemberExpr& node)
	{

	}

private:
	llvm::DIType createType(llvm::Type::TypeID& type, compile_unit_t& compile_unit)
	{
		if (compile_unit.type_cache.find(type) == compile_unit.type_cache.end())
		{
			// Create the specific type
			switch(type)
			{
			case llvm::Type::VoidTyID:	break;
			case llvm::Type::FloatTyID:	break;
			case llvm::Type::DoubleTyID:	break;
			case llvm::Type::X86_FP80TyID:	break;
			case llvm::Type::FP128TyID:	break;
			case llvm::Type::PPC_FP128TyID:	break;
			case llvm::Type::LabelTyID:	break;
			case llvm::Type::MetadataTyID:	break;
			case llvm::Type::IntegerTyID:
			{
				llvm::DIBasicType basic_type = factory.CreateBasicType(compile_unit.file,
						llvm::StringRef("int"), compile_unit.file,
						/* line */ 0, /* bits */32, /* alignment */ 32, /* offset */ 0, /*flags*/ 0,
						/* encoding */ llvm::dwarf::DW_ATE_signed);
				compile_unit.type_cache[type] = basic_type;
				break;
			}
			case llvm::Type::FunctionTyID:	break;
			case llvm::Type::StructTyID:	break;
			case llvm::Type::ArrayTyID:	break;
			case llvm::Type::PointerTyID:	break;
			case llvm::Type::OpaqueTyID:	break;
			case llvm::Type::VectorTyID:	break;
			default:
				return llvm::DIType();
			}
		}

		return compile_unit.type_cache[type];
	}


private:
	llvm::LLVMContext& context;
	llvm::Module& current_module;
	llvm::IRBuilder<> builder;
	LLVMHelper helper;

	llvm::DIFactory factory;

	std::vector<compile_unit_t> compile_units;
};

}}}}

#endif /* ZILLIANS_LANGUAGE_STAGE_VISITOR_LLVMDEBUGINFOGENERATORVISITOR_H_ */
