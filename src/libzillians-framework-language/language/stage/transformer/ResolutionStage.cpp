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

#include "language/stage/transformer/ResolutionStage.h"
#include "language/stage/transformer/visitor/ResolutionStageVisitor.h"
#include "language/tree/visitor/general/NodeTypeNameVisitor.h"
#include "language/tree/visitor/general/NodeInfoVisitor.h"
#include "language/tree/visitor/general/PrettyPrintVisitor.h"
#include "language/resolver/Resolver.h"
#include "language/context/ParserContext.h"
#include "language/tree/ASTNodeHelper.h"

namespace zillians { namespace language { namespace stage {

ResolutionStage::ResolutionStage() : debug(false), disable_type_inference(false), total_resolved_count(0), total_unresolved_count(0)
{ }

ResolutionStage::~ResolutionStage()
{ }

const char* ResolutionStage::name()
{
	return "Resolution Stage";
}

std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> ResolutionStage::getOptions()
{
	shared_ptr<po::options_description> option_desc_public(new po::options_description());
	shared_ptr<po::options_description> option_desc_private(new po::options_description());

	option_desc_public->add_options()
		("no-type-inference", "disable type inference system so every type declaration must be made explicitly");

	foreach(i, option_desc_public->options()) option_desc_private->add(*i);

	option_desc_private->add_options()
		("debug-resolution-stage", "debug type conversion stage");

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ResolutionStage::parseOptions(po::variables_map& vm)
{
	debug = (vm.count("debug-resolution-stage") > 0);
	disable_type_inference = (vm.count("no-type-inference") > 0);

	return true;
}

bool ResolutionStage::execute(bool& continue_execution)
{
	if(!hasParserContext())
		return false;

	bool complete_type_resolution = false;
	bool complete_symbol_resolution = false;

	while(true)
	{
		bool making_progress_on_type_resolution = false;
		if(!complete_type_resolution)
			complete_type_resolution = resolveTypes(false, making_progress_on_type_resolution);

		bool making_progress_on_symbol_resolution = false;
		if(!complete_symbol_resolution)
			complete_symbol_resolution = resolveSymbols(false, making_progress_on_symbol_resolution);

		if(complete_type_resolution && complete_symbol_resolution && !making_progress_on_type_resolution && !making_progress_on_symbol_resolution)
			break;
	}

	bool dummy = false;
	if(!complete_type_resolution)
		resolveTypes(true, dummy);
	if(!complete_symbol_resolution)
		resolveSymbols(true, dummy);

	if(debug)
	{
		tree::visitor::PrettyPrintVisitor printer;
		printer.visit(*getParserContext().program);
	}

	return complete_type_resolution && complete_symbol_resolution;
}

bool ResolutionStage::resolveTypes(bool report_error_summary, bool& making_progress)
{
	ParserContext& parser_context = getParserContext();

	if(!parser_context.program)
		return false;

	LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, L"trying to resolve types");

	tree::Program& program = *parser_context.program;

	Resolver resolver;
	visitor::ResolutionStageVisitor visitor(visitor::ResolutionStageVisitor::Target::TYPE_RESOLUTION, resolver);

	std::size_t last_unresolved_count = std::numeric_limits<std::size_t>::max();
	while(true)
	{
		visitor.reset();
		visitor.visit(program);

		total_resolved_count += visitor.getResolvedCount();
		std::size_t unresolved_count = visitor.getUnresolvedCount();

		if(unresolved_count == 0 || unresolved_count == last_unresolved_count)
		{
			if(unresolved_count == 0) last_unresolved_count = 0;

			if(!visitor.hasTransforms())
				break;
		}
		else
		{
			BOOST_ASSERT(unresolved_count < last_unresolved_count && "make sure we are making progress");
			last_unresolved_count = unresolved_count;
		}

		visitor.applyTransforms();
	}

	if(last_unresolved_count > 0)
	{
		total_unresolved_count += last_unresolved_count;

		if(report_error_summary)
		{
			LOG_MESSAGE(UNDEFINED_TYPE, (ASTNode*)NULL, _count = total_unresolved_count);

			for(__gnu_cxx::hash_set<ASTNode*>::iterator it = visitor.unresolved_nodes.begin(); it != visitor.unresolved_nodes.end(); ++it)
			{
				LOG_MESSAGE(UNDEFINED_TYPE_INFO, *it, _id = ASTNodeHelper::nodeName(*it));
				//zillians::language::LoggerWrapper::instance()->getLogger()->UNDEFINED_TYPE_INFO(zillians::language::_program_node = (getParserContext().program), zillians::language::_node = (*it), _id = ASTNodeHelper::nodeName(*it));
			}
		}

		return false;
	}

	return true;
}

bool ResolutionStage::resolveSymbols(bool report_error_summary, bool& making_progress)
{
	ParserContext& parser_context = getParserContext();

	if(!parser_context.program)
		return false;

	LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, "trying to resolve symbols");

	tree::Program& program = *parser_context.program;

	Resolver resolver;
	visitor::ResolutionStageVisitor visitor(visitor::ResolutionStageVisitor::Target::SYMBOL_RESOLUTION, resolver);

	std::size_t last_unresolved_count = std::numeric_limits<std::size_t>::max();
	while(true)
	{
		visitor.reset();
		visitor.visit(program);

		total_resolved_count += visitor.getResolvedCount();
		std::size_t unresolved_count = visitor.getUnresolvedCount();

		if(unresolved_count == 0 || unresolved_count == last_unresolved_count)
		{
			if(unresolved_count == 0) last_unresolved_count = 0;

			if(!visitor.hasTransforms())
				break;
		}
		else
		{
			BOOST_ASSERT(unresolved_count < last_unresolved_count && "make sure we are making progress");
			last_unresolved_count = unresolved_count;
		}

		visitor.applyTransforms();
	}

	if(last_unresolved_count > 0)
	{
		total_unresolved_count += last_unresolved_count;

		if(report_error_summary)
		{
			LOG_MESSAGE(UNDEFINED_SYMBOL, (ASTNode*)NULL, _count = total_unresolved_count);

			for(__gnu_cxx::hash_set<ASTNode*>::iterator it = visitor.unresolved_nodes.begin(); it != visitor.unresolved_nodes.end(); ++it)
			{
				LOG_MESSAGE(UNDEFINED_SYMBOL_INFO, (ASTNode*)*it, _id = ASTNodeHelper::nodeName(*it));
			}
		}

		return false;
	}

	return true;
}
//
//std::size_t ResolutionStage::get_resolved_count()
//{
//	return total_resolved_count;
//}
//
//std::size_t ResolutionStage::get_unresolved_count()
//{
//	return total_unresolved_count;
//}

} } }
