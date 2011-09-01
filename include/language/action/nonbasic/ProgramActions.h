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

#ifndef ZILLIANS_LANGUAGE_ACTION_MODULE_PROGRAMACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_MODULE_PROGRAMACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

namespace zillians { namespace language { namespace action {

struct program
{
	DEFINE_ATTRIBUTES(void)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
#ifdef DEBUG
		printf("program attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		getParserContext().program = new Program();
		getParserContext().active_package = getParserContext().program->root;
	}
	END_ACTION

	BEGIN_ACTION(append_package)
	{
#ifdef DEBUG
		printf("program::append_package_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		NestedIdentifier *nested_ident = cast<NestedIdentifier>(_attr(0));
		Package* prev_package = getParserContext().program->root;
		deduced_foreach_value(i, nested_ident->identifier_list)
		{
			Package *package = prev_package->findPackage(i->toString());
			if(!package)
			{
				package = new Package(cast<SimpleIdentifier>(i));
				prev_package->addPackage(package);
			}
			prev_package = package;
		}
		getParserContext().active_package = prev_package;
	}
	END_ACTION

	BEGIN_ACTION(append_import)
	{
#ifdef DEBUG
		printf("program::append_import_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		if(!!getParserContext().program)
			getParserContext().program->addImport(new Import(_attr(0)));
	}
	END_ACTION

	BEGIN_ACTION(append_declaration)
	{
#ifdef DEBUG
		printf("program::append_declaration attr(0) type = %s\n", typeid(_attr_t(0)).name());
#endif
		if(!!getParserContext().active_package)
			getParserContext().active_package->addObject(_attr(0));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_MODULE_PROGRAMACTIONS_H_ */
