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

#ifndef ZILLIANS_LANGUAGE_ACTION_MODULE_PACKAGEACTIONS_H_
#define ZILLIANS_LANGUAGE_ACTION_MODULE_PACKAGEACTIONS_H_

#include "language/action/detail/SemanticActionsDetail.h"

namespace zillians { namespace language { namespace action {

/**
 * package_decl is used to store AST nodes in a hierarchy
 *
 * for example, a package declaration like "com.zillians.test" will result in a nested package hierarchy like:
 * new Package(new SimpleIdentifier(L"com")).addPackage(new Package(new SimpleIdentifier(L"zillians")).addPackage(new Package(new SimpleIdentifier(L"test"))));
 */
struct package_decl
{
	DEFINE_ATTRIBUTES(Package*)
	DEFINE_LOCALS()

	BEGIN_ACTION(init)
	{
		BOOST_MPL_ASSERT(( boost::is_same<_value_t, Package*&> ));
		BOOST_MPL_ASSERT(( boost::is_same<_attr_t(0), NestedIdentifier*&> ));

		NestedIdentifier *nested_id = cast<NestedIdentifier>(_attr(0));
		Package* last = getParserContext().program->root;
		for(std::vector<Identifier*>::iterator id = nested_id->identifier_list.begin(); id != nested_id->identifier_list.end(); ++id)
		{
			Package *next = last->findPackage((*id)->toString());
			if(!next)
			{
				next = new Package(cast<SimpleIdentifier>(*id));
				last->addPackage(next);
			}
			last = next;
		}
		_value = last;
	}
	END_ACTION
};

} } }

#endif /* ZILLIANS_LANGUAGE_ACTION_MODULE_PACKAGEACTIONS_H_ */
