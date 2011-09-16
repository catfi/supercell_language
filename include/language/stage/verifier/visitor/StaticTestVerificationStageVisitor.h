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
#include "language/logging/StringTable.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/general/GenericDoubleVisitor.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/context/LogContext.h"

using namespace zillians::language::tree;
using zillians::language::tree::visitor::GenericDoubleVisitor;

namespace zillians { namespace language { namespace stage { namespace visitor {

struct StaticTestVerificationStageVisitor : public zillians::language::tree::visitor::GenericDoubleVisitor
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

	void check(zillians::language::tree::ASTNode& node)
	{
		revisit(node);
	}

	void check(zillians::language::tree::Annotation& node)
	{
		using zillians::language::stage::LogInfoContext;
		if (node.name->name == L"static_test")
		{
			// get error info context on node
			// the parent of Annotation is Annotations,
			// the error info context is hooked on the parent of Annotations.
			LogInfoContext* errorInfo = LogInfoContext::get(node.parent/*Annotations*/->parent);
			if(errorInfo == NULL)
			{
				mAllMatch = false;
				LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "No LogInfContext on node");
				return;
			}
			LogInfoContext constructedErrorInfo = constructErrorContextFromAnnotation(node);
			foreach(i, constructedErrorInfo.parameters)
			{
				if(!errorInfo->parameters.count(i->first))
				{
					LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "LogInfContext on node if different from the annotation on the node");
					mAllMatch = false;
					return;
				}
			}
		}
	}

private:
	zillians::language::stage::LogInfoContext constructErrorContextFromAnnotation(zillians::language::tree::Annotation& node)
	{
		int source_index = 0;
		//stage::ModuleSourceInfoContext* module_info = new stage::ModuleSourceInfoContext();
		//source_index = module_info->addSource("test.cpp");
		//source_index = module_info->addSource("hello.cpp");
		//stage::ModuleSourceInfoContext::set(&programNode, module_info);
		stage::SourceInfoContext::set(&node, new stage::SourceInfoContext(source_index, 32, 10) );

		using namespace zillians::language::tree;
		using zillians::language::tree::cast;
		//zillians::language::stage::LogInfoContext result;
		BOOST_ASSERT(node.attribute_list.size() == 1);
		if (node.attribute_list.size() != 1 )
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "number of attribute list should be 1");
		}

		std::pair<SimpleIdentifier*, ASTNode*>& expectMessage = node.attribute_list[0];
		BOOST_ASSERT(cast<SimpleIdentifier>(expectMessage.first)->name == L"expect_message");
		if (cast<SimpleIdentifier>(expectMessage.first) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be \"expect_message\"");
		}

		BOOST_ASSERT(cast<Annotation>(expectMessage.second) != NULL);
		if (cast<Annotation>(expectMessage.second) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "child of Annotations should be Annotation");
		}

		BOOST_ASSERT(cast<Annotation>(expectMessage.second)->attribute_list.size() == 3);
		if (cast<Annotation>(expectMessage.second)->attribute_list.size() != 3)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "number of attribute list should be 3");
		}

		// log level
		std::pair<SimpleIdentifier*, ASTNode*> logLevel = cast<Annotation>(expectMessage.second)->attribute_list[0];
		BOOST_ASSERT(cast<SimpleIdentifier>(logLevel.first) != NULL);
		if (cast<SimpleIdentifier>(logLevel.first) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be SimpleIdentifier");
		}

		BOOST_ASSERT(cast<SimpleIdentifier>(logLevel.first)->name == L"level");
		if (cast<SimpleIdentifier>(logLevel.first)->name != L"level")
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be \"level\"");
		}

		BOOST_ASSERT(cast<PrimaryExpr>(logLevel.second) != NULL);
		BOOST_ASSERT(cast<StringLiteral>(cast<PrimaryExpr>(logLevel.second)->value.literal) != NULL);
		if (cast<PrimaryExpr>(logLevel.second) == NULL || cast<StringLiteral>(cast<PrimaryExpr>(logLevel.second)->value.literal) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "value should be StringLiteral");
		}

		std::wstring levelString = cast<StringLiteral>(cast<PrimaryExpr>(logLevel.second)->value.literal)->value;

		// log id
		std::pair<SimpleIdentifier*, ASTNode*> logId = cast<Annotation>(expectMessage.second)->attribute_list[1];
		BOOST_ASSERT(cast<SimpleIdentifier>(logId.first) != NULL);
		if (cast<SimpleIdentifier>(logId.first) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be SimpleIdentifier");
		}

		BOOST_ASSERT(cast<SimpleIdentifier>(logId.first)->name == L"id");
		if (cast<SimpleIdentifier>(logId.first)->name != L"id")
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key shoule be \"id\"");
		}

		BOOST_ASSERT(cast<PrimaryExpr>(logId.second) != NULL);
		BOOST_ASSERT(cast<StringLiteral>(cast<PrimaryExpr>(logId.second)->value.literal) != NULL);
		if (cast<PrimaryExpr>(logId.second) == NULL || cast<StringLiteral>(cast<PrimaryExpr>(logId.second)->value.literal) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "value should be StringLiteral");
		}

		std::wstring idString = cast<StringLiteral>(cast<PrimaryExpr>(logId.second)->value.literal)->value;

		// parameter pairs
		std::pair<SimpleIdentifier*, ASTNode*> paramPairs = cast<Annotation>(expectMessage.second)->attribute_list[2];
		BOOST_ASSERT(cast<SimpleIdentifier>(paramPairs.first) != NULL);
		if (cast<SimpleIdentifier>(paramPairs.first) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be SimpleIdentifier");
		}

		BOOST_ASSERT(cast<SimpleIdentifier>(paramPairs.first)->name == L"parameters");
		if (cast<SimpleIdentifier>(paramPairs.first)->name != L"parameters")
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be \"parameters\"");
		}

		BOOST_ASSERT(cast<Annotation>(paramPairs.second) != NULL);
		if (cast<Annotation>(paramPairs.second) == NULL)
		{
			LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "value should be Annotation");
		}

		Annotation* params = cast<Annotation>(paramPairs.second);
		std::map<std::wstring, std::wstring> paramsResult;
		foreach(i, params->attribute_list)
		{
			SimpleIdentifier *paramKey = cast<SimpleIdentifier>(i->first);
			BOOST_ASSERT(paramKey != NULL);
			if (paramKey == NULL)
			{
				LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "key should be SimpleIdentifier");
			}

			BOOST_ASSERT(cast<PrimaryExpr>(i->second) != NULL);
			StringLiteral *paramValue = cast<StringLiteral>(cast<PrimaryExpr>(i->second)->value.literal);
			BOOST_ASSERT(paramValue != NULL);
			if (cast<PrimaryExpr>(i->second) == NULL || paramValue == NULL)
			{
				LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = node, _DETAIL = "value should be StringLiteral");
			}

			std::wstring key = paramKey->name;
			std::wstring value = paramValue->value;
			//zillians::language::stage::LogInfoContext::parameter_type_t param(key, value);
			std::pair<std::wstring, std::wstring> param(key, value);
			paramsResult.insert(std::make_pair(key, value));
		}

		return zillians::language::stage::LogInfoContext(levelString, idString, paramsResult);
	}

public:
	Program* programNode;
private:
	bool mAllMatch ;
};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_ERRORMESSAGEANNOTATIONCHECKVISITOR_H_ */
