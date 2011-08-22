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

#include "core/Prerequisite.h"
#include "../../ThorScriptTreeTest/ASTNodeSamples.h"
#include "../TransformerWrapper.h"
#include "language/stage/transformer/TypeInferenceStage.h"

#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTransformerTest_TypeInferenceTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::stage;

BOOST_AUTO_TEST_SUITE( ThorScriptTransformerTest_TypeInferenceTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTransformerTest_TypeInferenceTestCase1 )
{
	TransformerWrapper<TypeInferenceStage> wrapper;
	wrapper.setProgram(createSample4());
	BOOST_CHECK(wrapper.run() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
