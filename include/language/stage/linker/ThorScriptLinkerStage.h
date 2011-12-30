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

#ifndef ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTLINKERSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTLINKERSTAGE_H_

#include "language/stage/Stage.h"

namespace zillians { namespace language { namespace stage {

/**
 * The ThorScriptLinkerStage is responsible for generating native machine code
 */
class ThorScriptLinkerStage : public Stage
{
public:
	ThorScriptLinkerStage();
	virtual ~ThorScriptLinkerStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

private:
	bool buildAssemblyCode(const std::string& bc_file, std::string& asm_file);
	bool buildNativeCode(const std::vector<std::string>& asm_files);

private:
	std::vector<std::string> link_search_paths;
	std::vector<std::string> runtime_search_paths;

	std::vector<std::string> native_files;
	std::vector<std::string> bc_files;
	std::vector<std::string> link_libraries;
	std::string bundle_file;
	std::string output_file;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTLINKERSTAGE_H_ */
