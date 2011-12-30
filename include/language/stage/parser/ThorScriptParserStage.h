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

#ifndef ZILLIANS_LANGUAGE_STAGE_PARSER_THORSCRIPTPARSERSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_PARSER_THORSCRIPTPARSERSTAGE_H_

#include "language/stage/Stage.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include <boost/filesystem.hpp>

namespace zillians { namespace language { namespace stage {

class ThorScriptParserStage : public Stage
{
public:
	ThorScriptParserStage();
	virtual ~ThorScriptParserStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

private:
	bool parse(const boost::filesystem::path& p);

private:
	bool debug_parser;
	bool debug_ast;
	bool debug_ast_with_loc;
	bool use_relative_path;
	bool dump_graphviz;
    std::string dump_graphviz_dir;
    std::string prepand_package;
	boost::filesystem::path root_dir;
	std::vector<std::string> inputs;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_PARSER_THORSCRIPTPARSERSTAGE_H_ */
