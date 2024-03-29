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

#ifndef ZILLIANS_LANGUAGE_STAGE_STAGECONDUCTOR_H_
#define ZILLIANS_LANGUAGE_STAGE_STAGECONDUCTOR_H_

#include "core/Prerequisite.h"
#include "language/stage/Stage.h"

namespace zillians { namespace language { namespace stage {

class StageConductor
{
public:
	StageConductor(bool require_input);
	virtual ~StageConductor();

public:
	void appendStage(shared_ptr<Stage> s);
	void appendOptionsFromAllStages(po::options_description& options_desc_public, po::options_description& options_desc_private);

public:
	virtual int main(int argc, const char** argv);

protected:
	po::options_description mOptionDescGlobal;
	po::positional_options_description mPositionalOptionDesc;
	std::vector<shared_ptr<Stage>> mStages;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_STAGECONDUCTOR_H_ */
