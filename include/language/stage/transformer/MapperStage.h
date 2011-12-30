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

#ifndef ZILLIANS_LANGUAGE_STAGE_TRANSFORMER_MAPPERSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_TRANSFORMER_MAPPERSTAGE_H_

#include "core/Containers.h"
#include "language/stage/Stage.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/stage/transformer/visitor/MapperStageVisitor.h"

namespace zillians { namespace language { namespace stage {

class MapperStage : public Stage
{
public:
	MapperStage();
	virtual ~MapperStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

public:
	std::vector<std::string> mapper_ast_to_load;
	std::vector<ASTNode*> mapper_ast;
	std::vector<ClassDecl*> implicit_variables;
	unordered_map<std::string, FunctionDecl*> hashed_system_functions;
	visitor::MapperStageVisitor visitor;
	bool enabled;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_TRANSFORMER_MAPPERSTAGE_H_ */
