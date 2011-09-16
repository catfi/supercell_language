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

#include "language/stage/generator/LLVMBitCodeGeneratorStage.h"
#include "language/stage/generator/detail/LLVMForeach.h"
#include "language/context/ParserContext.h"
#include "language/context/GeneratorContext.h"

#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

namespace zillians { namespace language { namespace stage {

LLVMBitCodeGeneratorStage::LLVMBitCodeGeneratorStage() : emit_llvm(false), dump_llvm(false)
{ }

LLVMBitCodeGeneratorStage::~LLVMBitCodeGeneratorStage()
{ }

const char* LLVMBitCodeGeneratorStage::name()
{
	return "llvm_bitcode_generation_stage";
}

void LLVMBitCodeGeneratorStage::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{
    option_desc.add_options()
    ("emit-llvm", "emit llvm bitcode")
    ("dump-llvm", "dump llvm bitcode (stdout)");
}

bool LLVMBitCodeGeneratorStage::parseOptions(po::variables_map& vm)
{
	emit_llvm = (vm.count("emit-llvm") > 0);
	dump_llvm = (vm.count("dump-llvm") > 0);

	return true;
}

bool LLVMBitCodeGeneratorStage::execute(bool& continue_execution)
{
	if(dump_llvm)
	{
		if(!hasGeneratorContext())
			return false;

		GeneratorContext& generator_context = getGeneratorContext();
		foreach(m, generator_context.modules)
		{
			(*m)->dump();
		}
	}

	if(emit_llvm)
	{
		if(!hasGeneratorContext())
			return false;

		GeneratorContext& generator_context = getGeneratorContext();
		std::string error_info;

		foreach(m, generator_context.modules)
		{
			if(llvm::verifyModule(**m, llvm::PrintMessageAction, &error_info))
			{
				LOG4CXX_ERROR(LoggerWrapper::GeneratorStage, "failed to verify LLVM module: " << error_info);
			}
		}

		llvm::raw_fd_ostream stream("test.bc", error_info, llvm::raw_fd_ostream::F_Binary);
		if(stream.has_error())
		{
			LOG4CXX_ERROR(LoggerWrapper::GeneratorStage, "failed to prepare stream for writing LLVM bitcode: " << error_info);
			return false;
		}

		foreach(m, generator_context.modules)
		{
			llvm::WriteBitcodeToFile(*m, stream);
		}
	}

	return true;
}

} } }
