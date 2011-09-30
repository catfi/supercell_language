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

#ifndef ZILLIANS_LANGUAGE_STAGE_VERIFIER_SEMANTICVERIFICATIONSTAGE0_H_
#define ZILLIANS_LANGUAGE_STAGE_VERIFIER_SEMANTICVERIFICATIONSTAGE0_H_

#include "language/stage/Stage.h"
#include "language/resolver/Resolver.h"

namespace zillians { namespace language { namespace stage {

/**
 * The SemanticVerificationStage0 is run right after the parsing, so there's zero type information available.
 *
 * For all type-related semantic checks, we need to put it into SemanticVerificationStage1
 */
class SemanticVerificationStage0 : public Stage
{
public:
	SemanticVerificationStage0();
	virtual ~SemanticVerificationStage0();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_VERIFIER_SEMANTICVERIFICATIONSTAGE0_H_ */
