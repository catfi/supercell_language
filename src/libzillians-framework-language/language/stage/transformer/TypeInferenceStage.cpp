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

#include "language/stage/transformer/TypeInferenceStage.h"
#include "language/stage/transformer/visitor/TypeInferenceVisitor.h"
#include "language/resolver/TypeResolver.h"
#include "language/context/ParserContext.h"

namespace zillians { namespace language { namespace stage {

TypeInferenceStage::TypeInferenceStage() : disable_type_inference(false), total_resolved_count(0)
{ }

TypeInferenceStage::~TypeInferenceStage()
{ }

const char* TypeInferenceStage::name()
{
	return "type_inference";
}

void TypeInferenceStage::initializeOptions(po::options_description& option_desc, po::positional_options_description& positional_desc)
{
    option_desc.add_options()
    ("no-type-inference", "disable type inference system so every type declaration must be made explicitly");
}

bool TypeInferenceStage::parseOptions(po::variables_map& vm)
{
	disable_type_inference = (vm.count("no-type-inference") > 0);

	return true;
}

bool TypeInferenceStage::execute()
{
	if(!disable_type_inference)
	{
		if(getParserContext().program)
		{
			resolver::TypeResolver resolver;
			visitor::TypeInferenceVisitor visitor(resolver);

			std::size_t last_unresolved_count = 0;
			while(true)
			{
				visitor.reset_count();
				visitor.visit(*getParserContext().program);

				total_resolved_count += visitor.get_resolved_count();
				std::size_t unresolved_count = visitor.get_unresolved_count();

				if(unresolved_count == 0 || unresolved_count == last_unresolved_count)
				{
					break;
				}
				else
				{
					BOOST_ASSERT(unresolved_count < last_unresolved_count && "make sure we are making progress");
					last_unresolved_count = unresolved_count;
				}
			}

			if(last_unresolved_count > 0)
			{
				total_unresolved_count += last_unresolved_count;
				// TODO use log4cxx
				// there's unresolved types, failed
				//std::cerr << "failed to resolve types" << std::endl;
				LOG4CXX_ERROR(Logger::TransformerStage, L"failed to resolve types");
				return false;
			}
		}
		else
		{
			std::cerr << "empty program node" << std::endl;
		}
	}
	return true;
}

std::size_t TypeInferenceStage::get_resolved_count()
{
	return total_resolved_count;
}

std::size_t TypeInferenceStage::get_unresolved_count()
{
	return total_unresolved_count;
}


} } }
