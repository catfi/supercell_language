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
 *
 * @date Nov 7, 2010, yoco create init version
 */

#ifndef ZILLIANS_LANGUAGE_THORSCRIPTMANIFEST
#define ZILLIANS_LANGUAGE_THORSCRIPTMANIFEST

#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace zillians { namespace language {

struct ProjectDependency {
    std::vector<std::string> bundles ;
    std::vector<std::string> native_objects ;
    std::vector<std::string> native_libraries ;
} ;

struct ProjectManifest {
    typedef std::string key_type;
    std::string         name;
    std::string         author;
    std::string         version;
    ProjectDependency   dep;

    // read from xml
    void load(const std::string& filename) {
        using boost::property_tree::ptree ;
        ptree pt ;
        boost::property_tree::xml_parser::read_xml(filename, pt) ;

        // project
        name    = pt.get<std::string>("project.<xmlattr>.name") ;
        author  = pt.get<std::string>("project.<xmlattr>.author") ;
        version = pt.get<std::string>("project.<xmlattr>.version") ;

        // project dependency
        BOOST_FOREACH(ptree::value_type& v, pt.get_child("project.dependency")) {
            if(v.first == "bundle") {
                dep.bundles.push_back(v.second.get<std::string>("<xmlattr>.path"));
            } else if (v.first == "native_object") {
                dep.native_objects.push_back(v.second.get<std::string>("<xmlattr>.path"));
            } else if (v.first == "native_library") {
                dep.native_libraries.push_back(v.second.get<std::string>("<xmlattr>.path"));
            } else if (v.first == "<xmlcomment>") {
                // comment, do nothing
            } else {
                std::cerr << "Unrecognized xml tag `" << v.first << "` ignored" << std::endl;
            }
        }
    }
} ;

} } // namespace zillians language

#endif /*ZILLIANS_LANGUAGE_THORSCRIPTMANIFEST */
