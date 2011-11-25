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

#ifndef ZILLIANS_LANGUAGE_THORSCRIPTDRIVER_H_
#define ZILLIANS_LANGUAGE_THORSCRIPTDRIVER_H_

#include "language/tree/ASTNodeFactory.h"
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#include "language/ThorScriptManifest.h"

namespace zillians { namespace language {

class ThorScriptDriver
{
public:
    ThorScriptDriver();
    ~ThorScriptDriver();
    bool main(std::vector<std::string> argv) ;

private:
    enum class BUILD_TYPE {
        DEBUG,
        RELEASE
    } ;
    enum class STRIP_TYPE {
        STRIP,
        NO_STRIP
    } ;
    enum class STUB_LANG {
        CPP,
        JAVA
    } ;

private:
    bool createProjectSkeleton(const std::string& projectName);
    tree::Tangle* getMergedAST(const std::vector<std::string>& ast_files);

    bool buildDebug();
    bool buildRelease();
    bool build();
    bool generateBundle(const STRIP_TYPE isStrip);
    bool generateStub(const std::vector<std::string>& stubTypes);
    bool generateClientStub(const STUB_LANG);
    bool generateServerStub();

private:
    bool unbundle();
    bool dep();
    bool make(const BUILD_TYPE type);
    bool bundle();
    bool strip();
    bool link();

private:
    void saveCache(const std::string& s);
    std::string readCache();
    std::vector<std::string> getAstUnderBuild();
    bool setProjectPathAndBuildPath(std::vector<std::string>& argv);

protected:
    virtual int shell(const std::string& cmd);

private:
    boost::filesystem::path originalPath;
    zillians::language::ProjectManifest pm;
    boost::filesystem::path projectPath;
    boost::filesystem::path buildPath;
    std::string argv;
    bool dumpCommand;
};

} }

#endif /* ZILLIANS_LANGUAGE_THORSCRIPTDRIVER_H_ */
