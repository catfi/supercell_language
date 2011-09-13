/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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
/**
 * @date Aug 10, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_ERRORMESSAGEANNOTATIONCHECKVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_ERRORMESSAGEANNOTATIONCHECKVISITOR_H_

#include <string>
#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/context/StaticTestContext.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct StaticTestVerificationStageVisitor : public GenericDoubleVisitor
{
	CREATE_INVOKER(errorMessageAnnotationCheckInvoker, check);

	StaticTestVerificationStageVisitor() : mAllMatch(true)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(errorMessageAnnotationCheckInvoker)
	}

	bool isAllMatch() const
	{
		return mAllMatch;
	}

	void check(ASTNode& node)
	{
		revisit(node);
	}

	void check(Annotation& node)
	{
		using zillians::language::stage::ErrorInfoContext ;
		if (node.name->name == L"static_test")
		{
			// get error info context on node
			// the parent of Annotation is Annotations,
			// the error info context is hooked on the parent of Annotations.
			ErrorInfoContext* errorInfo = ErrorInfoContext::get(node.parent/*Annotations*/->parent);
			if(errorInfo == NULL)
			{
				mAllMatch = false;
				return;
			}
			ErrorInfoContext constructedErrorInfo = constructErrorContextFromAnnotation(node);
			// Compare (key, value) pairs only listed in annotation
			foreach(i, constructedErrorInfo.parameters)
			{
				if(!errorInfo->parameters.count(i->first))
				{
					mAllMatch = false;
					return;
				}
			}
		}
	}

private:
	zillians::language::stage::ErrorInfoContext constructErrorContextFromAnnotation(Annotation& node)
	{
		using zillians::language::tree::cast;
		//zillians::language::stage::ErrorInfoContext result;
		BOOST_ASSERT(node.attribute_list.size() == 1);
		// expect-message
		std::pair<SimpleIdentifier*, ASTNode*>& expectMessage = node.attribute_list[0];
		//BOOST_ASSERT(expectMessage != NULL);
		BOOST_ASSERT(cast<SimpleIdentifier>(expectMessage.first)->name == L"expect_message");
		BOOST_ASSERT(cast<Annotation>(expectMessage.second) != NULL);
		BOOST_ASSERT(cast<Annotation>(expectMessage.second)->attribute_list.size() == 3);

		// log level
		std::pair<SimpleIdentifier*, ASTNode*> logLevel = cast<Annotation>(expectMessage.second)->attribute_list[0];
		BOOST_ASSERT(cast<SimpleIdentifier>(logLevel.first) != NULL);
		BOOST_ASSERT(cast<SimpleIdentifier>(logLevel.first)->name == L"level");
		BOOST_ASSERT(cast<StringLiteral>(logLevel.second) != NULL);
		std::wstring levelString = cast<StringLiteral>(logLevel.second)->value;

		// log id
		std::pair<SimpleIdentifier*, ASTNode*> logId = cast<Annotation>(expectMessage.second)->attribute_list[1];
		BOOST_ASSERT(cast<SimpleIdentifier>(logId.first) != NULL);
		BOOST_ASSERT(cast<SimpleIdentifier>(logId.first)->name == L"id");
		BOOST_ASSERT(cast<StringLiteral>(logId.second) != NULL);
		std::wstring idString = cast<StringLiteral>(logId.second)->value;

		// parameter pairs
		std::pair<SimpleIdentifier*, ASTNode*> paramPairs = cast<Annotation>(expectMessage.second)->attribute_list[2];
		BOOST_ASSERT(cast<SimpleIdentifier>(paramPairs.first) != NULL);
		BOOST_ASSERT(cast<SimpleIdentifier>(paramPairs.first)->name == L"parameters");
		BOOST_ASSERT(cast<Annotation>(paramPairs.second) != NULL);
		Annotation* params = cast<Annotation>(paramPairs.second);
		std::map<std::wstring, std::wstring> paramsResult;
		foreach(i, params->attribute_list)
		{
			SimpleIdentifier *paramKey = cast<SimpleIdentifier>(i->first);
			BOOST_ASSERT(paramKey != NULL);
			StringLiteral *paramValue = cast<StringLiteral>(i->second);
			BOOST_ASSERT(paramValue != NULL);
			std::wstring key = paramKey->name;
			std::wstring value = paramValue->value;
			//zillians::language::stage::ErrorInfoContext::parameter_type_t param(key, value);
			std::pair<std::wstring, std::wstring> param(key, value);
			paramsResult.insert(std::make_pair(key, value));
		}

		return zillians::language::stage::ErrorInfoContext(levelString, idString, paramsResult);
	}

private:
	bool mAllMatch ;


};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_ERRORMESSAGEANNOTATIONCHECKVISITOR_H_ */
