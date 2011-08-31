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
		printf("program attr(0) type = %s\n", typeid(_attr_t(0)).name());
		getParserContext().program = new Program();
	}
	END_ACTION

	BEGIN_ACTION(append_package)
	{
		printf("program::append_package_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		NestedIdentifier *nested_id = cast<NestedIdentifier>(_attr(0));
		Package* last = getParserContext().program->root;
		deduced_foreach_value(i, nested_id->identifier_list)
		{
			Package *next = last->findPackage(i->toString());
			if(!next)
			{
				next = new Package(cast<SimpleIdentifier>(i));
				last->addPackage(next);
			}
			last = next;
		}
		getParserContext().active_package = last;
	}
	END_ACTION

	BEGIN_ACTION(append_import)
	{
		printf("program::append_import_decl attr(0) type = %s\n", typeid(_attr_t(0)).name());
		if(!!getParserContext().program)
			getParserContext().program->addImport(new Import(_attr(0)));
	}
	END_ACTION

	BEGIN_ACTION(append_declaration)
	{
		printf("program::append_declaration attr(0) type = %s\n", typeid(_attr_t(0)).name());
		if(!!getParserContext().active_package)
			getParserContext().active_package->addObject(_attr(0));
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_MODULE_PROGRAMACTIONS_H_ */
