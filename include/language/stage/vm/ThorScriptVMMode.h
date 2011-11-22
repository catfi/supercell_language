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

#ifndef ZILLIANS_LANGUAGE_STAGE_THORSCRIPTVMMODE_H_
#define ZILLIANS_LANGUAGE_STAGE_THORSCRIPTVMMODE_H_


#include "core/Prerequisite.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>

namespace zillians { namespace language { namespace stage {


/**
 * ThorScriptBaseVM
 */
class ThorScriptBaseVM
{
public:
	virtual ~ThorScriptBaseVM();

protected:
	ThorScriptBaseVM();

public:
	virtual bool parseOptions(boost::program_options::variables_map& vm) = 0;
	virtual bool execute() = 0;
};

/**
 * ThorScriptServerStandaloneVM
 */
class ThorScriptServerStandaloneVM : public ThorScriptBaseVM
{
public:
	ThorScriptServerStandaloneVM();
	virtual ~ThorScriptServerStandaloneVM();

public:
	bool parseOptions(boost::program_options::variables_map& vm);
	bool execute();
};

/**
 * ThorScriptServerSharedVM
 */
class ThorScriptServerSharedVM : public ThorScriptBaseVM
{
public:
	ThorScriptServerSharedVM();
	virtual ~ThorScriptServerSharedVM();

public:
	bool parseOptions(boost::program_options::variables_map& vm);
	bool execute();
};

/**
 * ThorScriptClientVM
 */
class ThorScriptClientVM : public ThorScriptBaseVM
{
public:
	ThorScriptClientVM();
	virtual ~ThorScriptClientVM();

public:
	bool parseOptions(boost::program_options::variables_map& vm);
	bool execute();

private:
	std::string getManglingName(std::string& name);

private:
	std::string module_name;
	std::string entry_symbol;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_THORSCRIPTVMMODE_H_ */
