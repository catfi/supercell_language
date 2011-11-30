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
//#include "language/tree/visitor/NodeTypeNameVisitor.h"
#include "language/tree/visitor/NodeInfoVisitor.h"
#include "language/tree/visitor/PrettyPrintVisitor.h"
#include "language/resolver/Resolver.h"
#include "language/context/ParserContext.h"
#include "language/tree/ASTNodeHelper.h"

namespace zillians { namespace language { namespace stage {

ResolutionStage::ResolutionStage() :
		debug(false),
		disable_type_inference(false),
		total_unresolved_count_type(std::numeric_limits<std::size_t>::max()),
		total_unresolved_count_symbol(std::numeric_limits<std::size_t>::max())
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
		("debug-resolution-stage", "debug type conversion stage")
		//("dump-graphviz", "dump AST in graphviz format")
		//("dump-graphviz-dir", po::value<std::string>(), "dump AST in graphviz format")
    ;

	return std::make_pair(option_desc_public, option_desc_private);
}

bool ResolutionStage::parseOptions(po::variables_map& vm)
{
	debug = (vm.count("debug-resolution-stage") > 0);
	disable_type_inference = (vm.count("no-type-inference") > 0);
	dump_graphviz = (vm.count("dump-graphviz") > 0);
    if(vm.count("dump-graphviz-dir") > 0)
    {
        dump_graphviz_dir = vm["dump-graphviz-dir"].as<std::string>();
    }

	return true;
}

bool ResolutionStage::execute(bool& continue_execution)
{
	UNUSED_ARGUMENT(continue_execution);

	if(!hasParserContext())
		return false;

	bool complete_type_resolution = false;
	bool complete_symbol_resolution = false;

    size_t count = 0;
	while(true)
	{
        if(dump_graphviz)
        {
            std::ostringstream oss;
            oss << "pre-resolution-" << count << ".dot";
            boost::filesystem::path p(dump_graphviz_dir);
            ASTNodeHelper::visualize(getParserContext().tangle, p / oss.str());
            ++count;
        }

		bool making_progress_on_type_resolution = false;
		if(!complete_type_resolution)
			complete_type_resolution = resolveTypes(false, making_progress_on_type_resolution);

		bool making_progress_on_symbol_resolution = false;
		if(!complete_symbol_resolution)
			complete_symbol_resolution = resolveSymbols(false, making_progress_on_symbol_resolution);

		if(!making_progress_on_type_resolution && !making_progress_on_symbol_resolution)
			break;
	}

	bool dummy = false;
	if(!complete_type_resolution)
		resolveTypes(true, dummy);
	if(!complete_symbol_resolution)
		resolveSymbols(true, dummy);

	// remove trivial and redundant errors
	removeTrivialErrors();

	// report errors
	reportErrors();

	if(debug)
	{
		tree::visitor::PrettyPrintVisitor printer;
		printer.visit(*getParserContext().active_source);
	}

	return complete_type_resolution && complete_symbol_resolution;
}

bool ResolutionStage::resolveTypes(bool report_error_summary, bool& making_progress)
{
	ParserContext& parser_context = getParserContext();

	if(!parser_context.active_source)
		return false;

	LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, L"resolution stage trying to resolve types");

	making_progress = false;

	Resolver resolver;
	visitor::ResolutionStageVisitor visitor(visitor::ResolutionStageVisitor::Target::TYPE_RESOLUTION, resolver);

	visitor.reset();
	visitor.visit(*parser_context.tangle);

	std::size_t unresolved_count = 0; 
	if(resolver.hasTransforms())
	{
		resolver.applyTransforms();
		making_progress = true;
        ++unresolved_count; // since we have instantiated new classes, there might be unresolved types within the class, so we have to assume there's at lease one unresolved types (though there can be none)
	}

	if(visitor.hasTransforms())
	{
		visitor.applyTransforms();
		making_progress = true;
	}

    unresolved_count += visitor.getUnresolvedCount();
	if(unresolved_count < total_unresolved_count_type)
	{
		total_unresolved_count_type = unresolved_count;
		making_progress = true;
	}

	if(unresolved_count != 0)
	{
		if(report_error_summary)
		{
			for(__gnu_cxx::hash_set<ASTNode*>::iterator it = visitor.unresolved_nodes.begin(); it != visitor.unresolved_nodes.end(); ++it)
			{
				unresolved_types.insert(*it);
			}
		}
		return false;
	}
	else
	{
		return true;
	}
}

bool ResolutionStage::resolveSymbols(bool report_error_summary, bool& making_progress)
{
	ParserContext& parser_context = getParserContext();

	if(!parser_context.active_source)
		return false;

	LOG4CXX_DEBUG(LoggerWrapper::TransformerStage, "resolution stage trying to resolve symbols");

	making_progress = false;

	Resolver resolver;
	visitor::ResolutionStageVisitor visitor(visitor::ResolutionStageVisitor::Target::SYMBOL_RESOLUTION, resolver);

	visitor.reset();
	visitor.visit(*parser_context.tangle);

	if(resolver.hasTransforms())
	{
		resolver.applyTransforms();
		making_progress = true;
	}

	if(visitor.hasTransforms())
	{
		visitor.applyTransforms();
		making_progress = true;
	}

	std::size_t unresolved_count = visitor.getUnresolvedCount();
	if(unresolved_count < total_unresolved_count_type)
	{
		total_unresolved_count_type = unresolved_count;
		making_progress = true;
	}

	if(unresolved_count != 0)
	{
		if(report_error_summary)
		{
			for(__gnu_cxx::hash_set<ASTNode*>::iterator it = visitor.unresolved_nodes.begin(); it != visitor.unresolved_nodes.end(); ++it)
			{
				unresolved_symbols.insert(*it);
			}
		}
		return false;
	}
	else
	{
		return true;
	}
}

void ResolutionStage::removeTrivialErrors()
{
	if(unresolved_symbols.size() > 0 || unresolved_types.size() > 0)
	{
		__gnu_cxx::hash_set<ASTNode*>::iterator it = unresolved_symbols.begin();
		std::vector<std::function<void()>> cleanup;

		while(it != unresolved_symbols.end())
		{
			// search for any ancestor of it in unresolved_symbols
			for(__gnu_cxx::hash_set<ASTNode*>::iterator i = unresolved_symbols.begin(); i != unresolved_symbols.end(); ++i)
			{
				ASTNode* parent = (*it)->parent;
				while(parent)
				{
					if(*i == parent)
					{
						cleanup.push_back([=]{
							unresolved_symbols.erase(parent);
						});
					}
					parent = parent->parent;
				}
			}

			// search for any ancestor of it in unresolved_types
			for(__gnu_cxx::hash_set<ASTNode*>::iterator i = unresolved_types.begin(); i != unresolved_types.end(); ++i)
			{
				ASTNode* parent = (*it)->parent;
				while(parent)
				{
					if(*i == parent)
					{
						cleanup.push_back([=]{
							unresolved_types.erase(parent);
						});
					}
					parent = parent->parent;
				}
			}

			// remove any ancestor of it since it's not leaf node, so the error is trivial due to unresolved descendant
			foreach(i, cleanup) (*i)();

			if(cleanup.size() > 0)
			{
				cleanup.clear();
				it = unresolved_symbols.begin();
			}
			else
			{
				++it;
			}
		}

		it = unresolved_types.begin();

		while(it != unresolved_types.end())
		{
			// search for any ancestor of it in unresolved_symbols
			for(__gnu_cxx::hash_set<ASTNode*>::iterator i = unresolved_symbols.begin(); i != unresolved_symbols.end(); ++i)
			{
				ASTNode* parent = (*it)->parent;
				while(parent)
				{
					if(*i == parent)
					{
						cleanup.push_back([=]{
							unresolved_symbols.erase(parent);
						});
					}
					parent = parent->parent;
				}
			}

			// search for any ancestor of it in unresolved_types
			for(__gnu_cxx::hash_set<ASTNode*>::iterator i = unresolved_types.begin(); i != unresolved_types.end(); ++i)
			{
				ASTNode* parent = (*it)->parent;
				while(parent)
				{
					if(*i == parent)
					{
						cleanup.push_back([=]{
							unresolved_types.erase(parent);
						});
					}
					parent = parent->parent;
				}
			}

			// remove any ancestor of it since it's not leaf node, so the error is trivial due to unresolved descendant
			foreach(i, cleanup) (*i)();

			if(cleanup.size() > 0)
			{
				cleanup.clear();
				it = unresolved_types.begin();
			}
			else
			{
				++it;
			}
		}
	}
}

void ResolutionStage::reportErrors()
{
	if(unresolved_symbols.size() > 0)
	{
		for(__gnu_cxx::hash_set<ASTNode*>::iterator it = unresolved_symbols.begin(); it != unresolved_symbols.end(); ++it)
		{
			// avoid duplicate error message if a symbol is not resolved
			if(unresolved_types.count(*it) > 0)
				unresolved_types.erase(*it);

			LOG_MESSAGE(UNDEFINED_SYMBOL_INFO, *it, _id = ASTNodeHelper::getNodeName(*it));
		}
	}

	if(unresolved_types.size() > 0)
	{
		for(__gnu_cxx::hash_set<ASTNode*>::iterator it = unresolved_symbols.begin(); it != unresolved_symbols.end(); ++it)
		{
			LOG_MESSAGE(UNDEFINED_TYPE_INFO, *it, _id = ASTNodeHelper::getNodeName(*it));
		}
	}
}

} } }
