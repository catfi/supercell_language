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
	return "LLVM BitCode Generation Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> LLVMBitCodeGeneratorStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("emit-llvm", po::value<std::string>(), "emit llvm bitcode")
		("dump-llvm", "dump llvm bitcode (stdout)");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options();

	return std::make_pair(option_desc_public, option_desc_private);
}

bool LLVMBitCodeGeneratorStage::parseOptions(po::variables_map& vm)
{
	emit_llvm = (vm.count("emit-llvm") > 0);
	if(emit_llvm)
	{
		llvm_bc_file = vm["emit-llvm"].as<std::string>();
	}
	dump_llvm = (vm.count("dump-llvm") > 0);

	return true;
}

bool LLVMBitCodeGeneratorStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

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

		llvm::raw_fd_ostream stream(llvm_bc_file.c_str(), error_info, llvm::raw_fd_ostream::F_Binary);
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
