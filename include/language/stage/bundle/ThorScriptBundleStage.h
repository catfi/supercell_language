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

#ifndef ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTBUNDLESTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTBUNDLESTAGE_H_

#include <vector>
#include <string>
#include "language/stage/Stage.h"

namespace zillians { namespace language { namespace stage {

/**
 * The ThorScriptBundleStage is responsible for:
 *
 * 1. Merge llvm bc files
 * 2. Merge ast trees into one
 * 3. Archive merged bc, ast file and a manifest as a bundle
 */
class ThorScriptBundleStage : public Stage
{
public:
	ThorScriptBundleStage();
	virtual ~ThorScriptBundleStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

private:
	void getMergeBitCodeBuffer(std::vector<unsigned char>& buffer);
	void getMergeASTBuffer(std::vector<unsigned char>& buffer);
	void getManifestBuffer(std::vector<unsigned char>& buffer);
    bool extract(bool& continue_execution);
    bool compress(bool& continue_execution);

public:
    std::vector<std::string> bitcode_files;
    std::vector<std::string> ast_files;
    std::string manifest_file;

    std::string output_file;
    std::vector<std::string> bundleDependency;
    bool stripped;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTBUNDLESTAGE_H_ */
