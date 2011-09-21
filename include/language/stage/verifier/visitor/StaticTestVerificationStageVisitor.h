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
#include "language/context/LogInfoContext.h"

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
		if (node.name->name == L"static_test")
		{
			// get error info context on node
			// the parent of Annotation is Annotations,
			// the error info context is hooked on the parent of Annotations.
			ASTNode* errorNode = node.parent->parent ;
			LogInfoContext* errorInfo = LogInfoContext::get(errorNode);
			if(errorInfo == NULL)
			{
				mAllMatch = false;
				LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, errorNode, _detail = L"No LogInfoContext on node");
				return;
			}
			LogInfoContext constructedErrorInfo = constructErrorContextFromAnnotation(node);
			if(errorInfo->log_id != constructedErrorInfo.log_id)
			{
				LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = *errorNode, _DETAIL = L"LogInfoContext on node is different from the annotation on the node: log id is different.");
			}
			if(errorInfo->log_level != constructedErrorInfo.log_level)
			{
				LoggerWrapper::instance()->getLogger()->WRONG_STATIC_TEST_ANNOTATION_FORMAT(_program_node = *programNode, _node = *errorNode, _DETAIL = L"LogInfoContext on node is different from the annotation on the node: log level is different.");
			}
			foreach(i, constructedErrorInfo.parameters)
			{
				if(!errorInfo->parameters.count(i->first))
				{
                    std::wstring errorDetail(L": missing expected key: '");
                    errorDetail += i->first ;
                    errorDetail += L"', existed: {" ;
                    foreach(j, errorInfo->parameters)
                    {
                        if(j != errorInfo->parameters.begin())
                        {
                            errorDetail += L", ";
                        }
                        errorDetail += j->first;
                    }
                    errorDetail += L"}" ;
                    LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, errorNode, _detail = L"LogInfoContext on node is different from the annotation on the node" + errorDetail);
					mAllMatch = false;
					return;
				}
			}
		}
	}

private:
    std::wstring getLiteralString(Literal* literal)
    {
        BOOST_ASSERT(isa<StringLiteral>(literal) || isa<NumericLiteral>(literal));
        if(isa<StringLiteral>(literal))
        {
            return cast<StringLiteral>(literal)->value;
        }
        else
        {
            NumericLiteral* numericLiteral = cast<NumericLiteral>(literal);
            std::wostringstream oss;
            switch(numericLiteral->type)
            {
            case PrimitiveType::type::BOOL    : oss << numericLiteral->value.b  ; break;
            case PrimitiveType::type::UINT8   : oss << numericLiteral->value.i8 ; break;
            case PrimitiveType::type::UINT16  : oss << numericLiteral->value.i16; break;
            case PrimitiveType::type::UINT32  : oss << numericLiteral->value.i32; break;
            case PrimitiveType::type::UINT64  : oss << numericLiteral->value.i64; break;
            case PrimitiveType::type::INT8    : oss << numericLiteral->value.u8 ; break;
            case PrimitiveType::type::INT16   : oss << numericLiteral->value.u16; break;
            case PrimitiveType::type::INT32   : oss << numericLiteral->value.u32; break;
            case PrimitiveType::type::INT64   : oss << numericLiteral->value.u64; break;
            case PrimitiveType::type::FLOAT32 : oss << numericLiteral->value.f32; break;
            case PrimitiveType::type::FLOAT64 : oss << numericLiteral->value.f64; break;
            default: break;
            }
            return oss.str();
        }
    }

	LogInfoContext constructErrorContextFromAnnotation(zillians::language::tree::Annotation& node)
	{
		using namespace zillians::language::tree;
		using zillians::language::tree::cast;

		BOOST_ASSERT(node.attribute_list.size() == 1);
		if (node.attribute_list.size() != 1 )
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"number of attribute list should be 1");
		}

		std::pair<SimpleIdentifier*, ASTNode*>& expectMessage = node.attribute_list[0];
		BOOST_ASSERT(cast<SimpleIdentifier>(expectMessage.first)->name == L"expect_message");
		if (cast<SimpleIdentifier>(expectMessage.first) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be \"expect_message\"");
		}

		BOOST_ASSERT(cast<Annotation>(expectMessage.second) != NULL);
		if (cast<Annotation>(expectMessage.second) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"child of Annotations should be Annotation");
		}

		Annotation* anno = cast<Annotation>(expectMessage.second);
		BOOST_ASSERT(cast<Annotation>(expectMessage.second)->attribute_list.size() == 3 ||
                     cast<Annotation>(expectMessage.second)->attribute_list.size() == 2);
		if (cast<Annotation>(expectMessage.second)->attribute_list.size() < 2 ||
		    cast<Annotation>(expectMessage.second)->attribute_list.size() > 3)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"number of attribute list should be 3 or 2");
		}

		// log level
		std::pair<SimpleIdentifier*, ASTNode*> logLevel = cast<Annotation>(expectMessage.second)->attribute_list[0];
		BOOST_ASSERT(cast<SimpleIdentifier>(logLevel.first) != NULL);
		if (cast<SimpleIdentifier>(logLevel.first) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be SimpleIdentifier");
		}

		BOOST_ASSERT(cast<SimpleIdentifier>(logLevel.first)->name == L"level");
		if (cast<SimpleIdentifier>(logLevel.first)->name != L"level")
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be \"level\"");
		}

		BOOST_ASSERT(cast<PrimaryExpr>(logLevel.second) != NULL);
		BOOST_ASSERT(cast<StringLiteral>(cast<PrimaryExpr>(logLevel.second)->value.literal) != NULL);
		if (cast<PrimaryExpr>(logLevel.second) == NULL || cast<StringLiteral>(cast<PrimaryExpr>(logLevel.second)->value.literal) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"value should be StringLiteral");
		}

		std::wstring levelString = cast<StringLiteral>(cast<PrimaryExpr>(logLevel.second)->value.literal)->value;

		// log id
		std::pair<SimpleIdentifier*, ASTNode*> logId = cast<Annotation>(expectMessage.second)->attribute_list[1];
		BOOST_ASSERT(cast<SimpleIdentifier>(logId.first) != NULL);
		if (cast<SimpleIdentifier>(logId.first) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be SimpleIdentifier");
		}

		BOOST_ASSERT(cast<SimpleIdentifier>(logId.first)->name == L"id");
		if (cast<SimpleIdentifier>(logId.first)->name != L"id")
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key shoule be \"id\"");
		}

		BOOST_ASSERT(cast<PrimaryExpr>(logId.second) != NULL);
		BOOST_ASSERT(cast<StringLiteral>(cast<PrimaryExpr>(logId.second)->value.literal) != NULL);
		if (cast<PrimaryExpr>(logId.second) == NULL || cast<StringLiteral>(cast<PrimaryExpr>(logId.second)->value.literal) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"value should be StringLiteral");
		}

		std::wstring idString = cast<StringLiteral>(cast<PrimaryExpr>(logId.second)->value.literal)->value;

		if (anno->attribute_list.size() == 2)
		{
			return LogInfoContext(levelString, idString, std::map<std::wstring, wstring>());
		}

		// parameter pairs
		std::pair<SimpleIdentifier*, ASTNode*> paramPairs = cast<Annotation>(expectMessage.second)->attribute_list[2];
		BOOST_ASSERT(cast<SimpleIdentifier>(paramPairs.first) != NULL);
		if (cast<SimpleIdentifier>(paramPairs.first) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be SimpleIdentifier");
		}

		BOOST_ASSERT(cast<SimpleIdentifier>(paramPairs.first)->name == L"parameters");
		if (cast<SimpleIdentifier>(paramPairs.first)->name != L"parameters")
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be \"parameters\"");
		}

		BOOST_ASSERT(cast<Annotation>(paramPairs.second) != NULL);
		if (cast<Annotation>(paramPairs.second) == NULL)
		{
			LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"value should be Annotation");
		}

		Annotation* params = cast<Annotation>(paramPairs.second);
		std::map<std::wstring, std::wstring> paramsResult;
		foreach(i, params->attribute_list)
		{
			SimpleIdentifier *paramKey = cast<SimpleIdentifier>(i->first);
			BOOST_ASSERT(paramKey != NULL);
			if (paramKey == NULL)
			{
				LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"key should be SimpleIdentifier");
			}

			BOOST_ASSERT(cast<PrimaryExpr>(i->second) != NULL);
            Literal* literal = cast<PrimaryExpr>(i->second)->value.literal;
            BOOST_ASSERT(cast<StringLiteral>(literal) != NULL || cast<NumericLiteral>(literal) != NULL);
			if (cast<PrimaryExpr>(i->second) == NULL || literal == NULL)
			{
				LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"value should be a StringLiteral or a NumericLiteral");
			}

			std::wstring key = paramKey->name;
			std::wstring value = getLiteralString(literal);
			std::pair<std::wstring, std::wstring> param(key, value);
			paramsResult.insert(std::make_pair(key, value));
		}

		return LogInfoContext(levelString, idString, paramsResult);
	}

public:
	Program* programNode;

private:
	bool mAllMatch;
};

} } } } // namespace zillians::language::tree::visitor

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_ERRORMESSAGEANNOTATIONCHECKVISITOR_H_ */
