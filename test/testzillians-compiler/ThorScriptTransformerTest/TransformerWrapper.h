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

#ifndef TRANSFORMERWRAPPER_H_
#define TRANSFORMERWRAPPER_H_

#include "language/stage/StageConductor.h"
#include "language/stage/basic/TreeDebugStage.h"
#include "language/context/ParserContext.h"

using namespace zillians::language;
using namespace zillians::language::stage;
using namespace zillians::language::tree;

struct TransformerWrapper : public StageConductor
{
	TransformerWrapper()
	{ }

	virtual ~TransformerWrapper()
	{ }

	virtual void initialize()
	{
		shared_ptr<TreeDebugStage> s1(new TreeDebugStage());
		appendStage(s1);
	}

	virtual void finalize()
	{ }

	void setProgram(ASTNode* program)
	{
		getParserContext().program = cast<Program>(program);
	}

	int run()
	{
		const char* argv[] = { "TransformerWrapper", "--dump-ast" };
		return main(2, argv);
	}
};


#endif /* TRANSFORMERWRAPPER_H_ */
