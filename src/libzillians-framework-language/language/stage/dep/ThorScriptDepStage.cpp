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

#include <algorithm>
#include <boost/filesystem.hpp>
#include "language/stage/dep/ThorScriptDepStage.h"

namespace zillians { namespace language { namespace stage {

ThorScriptDepStage::ThorScriptDepStage()
{ }

ThorScriptDepStage::~ThorScriptDepStage()
{ }

const char* ThorScriptDepStage::name()
{
	return "thor_script_dep_stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ThorScriptDepStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options() ;

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options() ;

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ThorScriptDepStage::parseOptions(po::variables_map& vm)
{
    if(vm.count("input"))
    {
        inputFiles = vm["input"].as<std::vector<PathNameType>>();
        return true;
    }
    else
    {
        return false;
    }
}

typedef std::wstring PathNameType ;
typedef std::map<std::wstring, std::set<PathNameType>> DepType ;
void addDeps(const PathNameType& filename, DepType& deps);

bool ThorScriptDepStage::execute(bool& continue_execution)
{
    DepType deps;

    foreach(i, inputFiles)
    {
        std::wstring s = *i ;
        std::wcout << "Parsing file: " << s << std::endl;
        addDeps(*i, deps);
    }
    return true;
}

bool isDirectory(const PathNameType& pathString)
{
    namespace fs = boost::filesystem;
    fs::path p(pathString);
    return fs::exists(p) && fs::is_directory(p);
}

bool isRegularFile(const PathNameType& pathString)
{
    namespace fs = boost::filesystem;
    fs::path p(pathString + L".t");
    return fs::exists(p) && fs::is_regular_file(p);
}

PathNameType packageNameToPathName(const std::wstring& packageName)
{
    PathNameType result;
    std::replace(result.begin(), result.end(), L'.', L'/');
    return result;
}

// TODO implement
std::set<std::wstring> getFileDependentPackages(const std::wstring& tFileName)
{
    return std::set<std::wstring>();
}

// TODO
std::set<PathNameType> recursiveAllFiles(std::wstring& dirPath)
{
    return std::set<PathNameType>();
}

void addDeps(const PathNameType& pathString, DepType& deps)
{
    namespace fs = boost::filesystem;
    // if file processed
    if(deps.count(pathString))
    {
        return;
    }

    deps[pathString + L".t"] = std::set<PathNameType>();

    // if is dir name
    fs::path p(pathString);
    if(isDirectory(pathString))
    {
        for (auto i = fs::directory_iterator(p); i != fs::directory_iterator(); ++i)
        {
            addDeps(i->path().wstring(), deps);
        }
    }

    // if is .t src file
    if(isRegularFile(pathString))
    {
        std::set<std::wstring> fileDependentPackages = getFileDependentPackages(p.wstring());
        std::vector<PathNameType> fileDependentPathNames(fileDependentPackages.size());
        std::transform(fileDependentPackages.begin(), fileDependentPackages.end(), fileDependentPathNames.begin(), packageNameToPathName);
        // for each dep packages
        foreach(i, fileDependentPathNames)
        {
            // if is regular .t file, directly add to deps
            if(isRegularFile(*i))
            {
                deps[pathString + L".t"].insert(*i);
            }
            // if is package directory, recusively add all files under the dir.
            else if(isDirectory(*i))
            {
                std::set<PathNameType> allFilesUnderDir = recursiveAllFiles(*i);
                deps[pathString + L".t"].insert(allFilesUnderDir.begin(), allFilesUnderDir.end());
            }
        }
    }
}

} } }

