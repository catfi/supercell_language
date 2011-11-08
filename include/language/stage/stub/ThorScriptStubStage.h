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

#ifndef ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTSTUBSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTSTUBSTAGE_H_

#include "language/stage/Stage.h"
#include "language/tree/ASTNode.h"
#include <vector>
#include <map>
#include <string>

namespace zillians { namespace language { namespace stage {

/**
 * The ThorScriptStubStage is responsible for:
 *
 * 1. generate stubs based on stub templates
 */
class ThorScriptStubStage : public Stage
{
public:
	ThorScriptStubStage();
	virtual ~ThorScriptStubStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

public:
    std::vector<std::string> ast_files;

    std::string output_file;
    typedef std::map<std::wstring, std::wstring> var_map_t;
    var_map_t var_map;

public:
	typedef enum
	{
		UNKNOWN_STUB,
		GAMENAME_CLIENTCOMMANDOBJECT_H,
		GAMENAME_CLOUDCOMMANDOBJECT_H,
		GAMENAME_GAMECOMMANDTRANSLATOR_CPP,
		GAMENAMEGAMEMODULE_MODULE
	} stub_type_t;

private:
	stub_type_t stub_type;
};

} } }

template<zillians::language::stage::ThorScriptStubStage::stub_type_t ENUM>
void genStub(zillians::language::tree::Tangle* node, zillians::language::stage::ThorScriptStubStage::var_map_t& var_map)
{ }

#endif /* ZILLIANS_LANGUAGE_STAGE_MAKE_THORSCRIPTSTUBSTAGE_H_ */
