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
 *
 * @date Oct 3, 2011 yoco - Initial version created.
 */

#include <iostream>
#include <vector>
#include "language/tree/ASTNodeFactory.h"
#include "language/stage/verifier/visitor/StaticTestVerificationStageVisitor.h"
#include "language/context/LogInfoContext.h"

namespace zillians { namespace language { namespace stage { namespace visitor {

//////////////////////////////////////////////////////////////////////////////
// Static function implementation

static void printLogInfoVec(const std::vector<LogInfo>& logInfoVec)
{
    for(std::vector<LogInfo>::size_type i = 0; i != logInfoVec.size(); ++i)
    {
        std::wcout << L"  [" << i + 1 << L'/' << logInfoVec.size() << L']' << std::endl ;
        std::wcout << L"    log_level : " << logInfoVec[i].log_level << std::endl ;
        std::wcout << L"    log_id    : " << logInfoVec[i].log_id << std::endl ;
        std::wcout << L"    parameters: " ;
        foreach(j ,logInfoVec[i].parameters)
        {
            if(j != logInfoVec[i].parameters.begin())
            {
                std::wcout << "                " ;
            }
            std::wcout << j->first << ": " << j->second << std::endl ;
        }
    }
}

static void dumpMultipleMismatchLogInfoError(ASTNode* errorNode, const std::vector<LogInfo>& annotatedLogInfo, const std::vector<LogInfo>& hookedLogInfo)
{
    std::wcout << L"Rest mismatched hooked LogInfo on ASTNode and annotation." << std::endl ;

    std::wcout << L"LogInfo on ASTNode: " << hookedLogInfo.size() << std::endl ;
    printLogInfoVec(hookedLogInfo);

    std::wcout << L"LogInfo on annotation: " << annotatedLogInfo.size() << std::endl ;
    printLogInfoVec(annotatedLogInfo);

}

static bool compareLogInfo(const LogInfo& annotatedLogInfo, const LogInfo& hookedLogInfo)
{
    if(hookedLogInfo.log_id != annotatedLogInfo.log_id)
    {
        return false;
    }
    if(hookedLogInfo.log_level != annotatedLogInfo.log_level)
    {
        return false;
    }
    foreach(i, annotatedLogInfo.parameters)
    {
        if(!hookedLogInfo.parameters.count(i->first))
        {
            return false;
        }
    }
    return true;
}

static bool compareOneAnnoToAllHook(const LogInfo& annotatedLogInfo, std::vector<LogInfo>& hookedLogInfoVec)
{
    for(size_t i = 0; i != hookedLogInfoVec.size(); ++i)
    {
        if(compareLogInfo(annotatedLogInfo, hookedLogInfoVec[i]))
        {
            hookedLogInfoVec.erase(hookedLogInfoVec.begin() + i);
            return true;
        }
    }
    return false;
}

static std::wstring getLiteralString(Literal* literal)
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
        case PrimitiveType::type::INT8    : oss << numericLiteral->value.i8 ; break;
        case PrimitiveType::type::INT16   : oss << numericLiteral->value.i16; break;
        case PrimitiveType::type::INT32   : oss << numericLiteral->value.i32; break;
        case PrimitiveType::type::INT64   : oss << numericLiteral->value.i64; break;
        case PrimitiveType::type::FLOAT32 : oss << numericLiteral->value.f32; break;
        case PrimitiveType::type::FLOAT64 : oss << numericLiteral->value.f64; break;
        default: break;
        }
        return oss.str();
    }
}

static LogInfo constructErrorContextFromAnnotation(zillians::language::tree::Annotation& node)
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
        return LogInfo(levelString, idString, std::map<std::wstring, wstring>());
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

    return LogInfo(levelString, idString, paramsResult);
}

//////////////////////////////////////////////////////////////////////////////
// Member function implement

std::vector<LogInfo> StaticTestVerificationStageVisitor::constructLogInfoVecFromAnnotations(zillians::language::tree::Annotations* annos)
{
    std::vector<LogInfo> annotatedLogInfoVec;
    if(annos == NULL)
    {
        return annotatedLogInfoVec;
    }

    foreach(i, annos->annotation_list)
    {
        if((*i)->name->name == L"static_test")
        {
            LogInfo logInfo = constructErrorContextFromAnnotation(**i);
            annotatedLogInfoVec.push_back(logInfo);
        }
    }
    return annotatedLogInfoVec;
}

bool StaticTestVerificationStageVisitor::compareLogInfoVec(ASTNode* errorNode, std::vector<LogInfo> annotatedLogInfoVec, std::vector<LogInfo> hookedLogInfoVec)
{
    if(annotatedLogInfoVec.size() != hookedLogInfoVec.size())
    {
        std::wostringstream oss;
        oss << L": Number of LogInfo mismatch, node is " << hookedLogInfoVec.size() << L", annotation is " << annotatedLogInfoVec.size() ;
        LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, errorNode, _detail = oss.str());
    }

    for(int i = annotatedLogInfoVec.size() - 1; i >= 0; --i)
    {
        const LogInfo& annotatedLogInfo = annotatedLogInfoVec[i];
        if(compareOneAnnoToAllHook(annotatedLogInfo, hookedLogInfoVec))
        {
            annotatedLogInfoVec.erase(annotatedLogInfoVec.begin() + i);
        }
    }

    if(annotatedLogInfoVec.size() == 0 && hookedLogInfoVec.size() == 0)
    {
        return true;
    }
    else
    {
        dumpMultipleMismatchLogInfoError(errorNode, annotatedLogInfoVec, hookedLogInfoVec);
        return false;
    }
}

} } } } // namespace zillians::language::tree::visitor
