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

#ifndef ZILLIANS_LANGUAGE_STAGE_TRANSFORMER_RESOLUTIONSTAGE_H_
#define ZILLIANS_LANGUAGE_STAGE_TRANSFORMER_RESOLUTIONSTAGE_H_

#include "language/stage/Stage.h"
#include "language/resolver/Resolver.h"

namespace zillians { namespace language { namespace stage {

/**
 * ResolutionStage tries to resolve types and symbols for all nodes in the AST by using multiple passes
 */
class ResolutionStage : public Stage
{
public:
	ResolutionStage();
	virtual ~ResolutionStage();

public:
	virtual const char* name();
	virtual std::pair<shared_ptr<po::options_description>, shared_ptr<po::options_description>> getOptions();
	virtual bool parseOptions(po::variables_map& vm);
	virtual bool execute(bool& continue_execution);

private:
	void resolveTypes(bool report_error_summary, bool& making_progress, bool& complete_type_resolution, bool& complete_symbol_resolution);
	void resolveSymbols(bool report_error_summary, bool& making_progress, bool& complete_type_resolution, bool& complete_symbol_resolution);

private:
	void removeTrivialErrors();
	void reportErrors();

private:
	bool debug;
	bool disable_type_inference;
	std::size_t total_unresolved_count_type;
	std::size_t total_unresolved_count_symbol;

	unordered_set<tree::ASTNode*> unresolved_symbols;
	unordered_set<tree::ASTNode*> unresolved_types;
	bool dump_graphviz;
	std::string dump_graphviz_dir;
	bool keep_going;
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_TRANSFORMER_RESOLUTIONSTAGE_H_ */
