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
#include <map>
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
        if(logInfoVec[i].parameters.empty())
        {
            std::wcout << L"(NONE)" << std::endl ;
        }
        else {
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

static LogInfo constructExpectMessageFromAnnotation(zillians::language::tree::ASTNode& node)
{
    using namespace zillians::language::tree;
    using zillians::language::tree::cast;

    ASTNode* expectMessageAnno = &node;

    BOOST_ASSERT(cast<Annotation>(expectMessageAnno) != NULL);
    if (cast<Annotation>(expectMessageAnno) == NULL)
    {
        LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"child of Annotations should be Annotation");
    }

    Annotation* anno = cast<Annotation>(expectMessageAnno);
    BOOST_ASSERT(cast<Annotation>(expectMessageAnno)->attribute_list.size() == 3 ||
                 cast<Annotation>(expectMessageAnno)->attribute_list.size() == 2);
    if (cast<Annotation>(expectMessageAnno)->attribute_list.size() < 2 ||
        cast<Annotation>(expectMessageAnno)->attribute_list.size() > 3)
    {
        LOG_MESSAGE(WRONG_STATIC_TEST_ANNOTATION_FORMAT, &node, _detail = L"number of attribute list should be 3 or 2");
    }

    // log level
    std::pair<SimpleIdentifier*, ASTNode*> logLevel = cast<Annotation>(expectMessageAnno)->attribute_list[0];
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
    std::pair<SimpleIdentifier*, ASTNode*> logId = cast<Annotation>(expectMessageAnno)->attribute_list[1];
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
    std::pair<SimpleIdentifier*, ASTNode*> paramPairs = cast<Annotation>(expectMessageAnno)->attribute_list[2];
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
            Annotation& anno = **i;
            foreach(j, anno.attribute_list)
            {
                std::pair<SimpleIdentifier*, ASTNode*>& attr = *j;
                if(attr.first->name == L"expect_message")
                {
                    LogInfo logInfo = constructExpectMessageFromAnnotation(*attr.second);
                    annotatedLogInfoVec.push_back(logInfo);
                }
            }
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

/**
 * @brief Get the expect_resolution id of the function call
 * @param node The ExpressionStmt node of the function call
 * @return @p __no_annotation__ if there is no expect_resolution annotation on this node.
 *         @p (EmptyString) if the function call is not resolved
 *         @p SomeId if the function is resolved
 */
static std::wstring getUseId(ASTNode& node)
{
    Annotation* useAnno = ASTNodeHelper::findAnnotation(&node, L"static_test");
    if(useAnno == NULL)
    {
        return L"__no_annotation__";
    }

    foreach(i, useAnno->attribute_list)
    {
        SimpleIdentifier* useAnnoKey = cast<SimpleIdentifier>(i->first);
        if(useAnnoKey->name == L"expect_resolution")
        {
            PrimaryExpr* useAnnoValue = cast<PrimaryExpr>(i->second);
            const std::wstring& useId = cast<StringLiteral>(useAnnoValue->value.literal)->value;
            return useId;
        }
    }
    return L"__no_annotation__";
}


static Declaration* getDecl(ASTNode& node)
{
    ASTNode*         stmtNode = &node;
    ExpressionStmt*  exprStmt = cast<ExpressionStmt>(stmtNode);
    DeclarativeStmt* declStmt = cast<DeclarativeStmt>(stmtNode);
    // function call
    if(exprStmt != NULL)
    {
        CallExpr*    callExpr = cast<CallExpr>(exprStmt->expr);
        if(ResolvedSymbol::get(callExpr->node) == NULL) return NULL;
        Declaration* decl     = cast<Declaration>(ResolvedSymbol::get(callExpr->node));
        return decl;
    }
    // variable declaration
    else
    {
        VariableDecl*  varDecl = cast<VariableDecl>(declStmt->declaration);
        TypeSpecifier* type    = varDecl->type;
        if(ResolvedType::get(type) == NULL) return NULL;
        Declaration*   decl    = cast<Declaration>(ResolvedType::get(type));
        return decl;
    }
}

/**
 * @brief Get the resolution id of the function declaration
 * @param node The ExpressionStmt node of the function call
 * @return @p __no_ResolvedSymbol__ if the function call is not resolved
 *         @p _no_StaticTestAnnotation_ if the declaration has no static_test annotation
 *         @p SomeId The declaration id if the function is resolved
 */
static std::wstring getDeclId(ASTNode& node)
{
    Declaration* decl = getDecl(node);
    if(decl == NULL)
    {
        return L"__no_ResolvedSymbol__";
    }

    Annotation*         declAnno      = ASTNodeHelper::findAnnotation(decl, L"static_test");
    if(declAnno == NULL)
    {
        return L"__no_annotation__";
    }

    PrimaryExpr*        declAnnoValue = cast<PrimaryExpr>(declAnno->attribute_list.begin()->second);
    const std::wstring& declId        = cast<StringLiteral>(declAnnoValue->value.literal)->value;
    return declId;
}

struct CodePoint
{
    CodePoint(ASTNode* node)
    {
        Source* sourceNode = ASTNodeHelper::getOwner<Source>(node);
        filename = s_to_ws(sourceNode->filename);
        stage::SourceInfoContext* source_info = stage::SourceInfoContext::get(node);
        line = source_info->line;
        column = source_info->column;
    }

    std::wstring filename;
    uint32 line;
    uint32 column;
} ;

/**
 * @brief Check function resolution
 * @param node function call stmt to be resolved
 * @return @p true if all match, @p false is not
 *
 * There are 3 OK:
 *   1. No annotation
 *   2. Can not be resolved && no ResolvedSymbol
 *   3. Can be resolved && has match ResolvedSymbol
 *
 * and 2 Fail:
 *   1. Fales positive: should not resolved, but resolved.
 *   2. Resolved, but not match
 */
bool StaticTestVerificationStageVisitor::checkResolutionTarget(zillians::language::tree::ASTNode& node)
{
    std::wstring useId = getUseId(node);

    // There are three OK:
    // 1. no annotation
    // 2. Can not be resolved && no ResolvedSymbol
    // 3. Can be resolved && has match ResolvedSymbol
    if(useId == L"__no_annotation__")
    {
        return true;
    }

    std::wstring callOrDecl = isa<ExpressionStmt>(&node) ? L"Function call" : L"Variable declaration";
    std::wstring declId = getDeclId(node);

    if(useId == L"" && declId == L"__no_ResolvedSymbol__")
    {
        std::cout << "Resolution OK" << std::endl;
        return true;
    }
    else if(useId == declId)
    {
        std::cout << "Resolution OK" << std::endl;
        return true;
    }

    // And three FAIL:
    // 1. Fales positive: should not resolved, but resolved.
    // 2. Should be resolved, but not.
    // 3. Resolved, but not match
    else if(useId == L"" && declId != L"__no_ResolvedSymbol__")
    {
        CodePoint usePoint(&node);
        CodePoint declPoint(getDecl(node));
        std::wcerr <<  usePoint.filename << L":" <<  usePoint.line << L": Error: " << callOrDecl << " `" << ASTNodeHelper::getNodeName(&node, true) << L"' should not be resolved, but resolved to:" << std::endl
                   << declPoint.filename << L":" << declPoint.line << L": " << ASTNodeHelper::getNodeName(getDecl(node), true) << std::endl;
        return false;
    }
    else if(useId != L"" && declId == L"__no_ResolvedSymbol__")
    {
        CodePoint usePoint(&node);
        std::wcerr <<  usePoint.filename << L":" <<  usePoint.line << L": Error: " << callOrDecl << " `" << ASTNodeHelper::getNodeName(&node, true) << L"' should be resolved, but not" << std::endl;
        return false;
    }
    else if(useId != declId)
    {
        CodePoint usePoint(&node);
        CodePoint declPoint(getDecl(node));
        std::wcerr <<  usePoint.filename << L":" <<  usePoint.line << L": Error: " << callOrDecl << " `" << ASTNodeHelper::getNodeName(&node, true) << L"' resolved to wrong declaration:" << std::endl
                   << declPoint.filename << L":" << declPoint.line << L": " << ASTNodeHelper::getNodeName(getDecl(node), true) << std::endl;
        return false;
    }

    UNREACHABLE_CODE();
}

} } } } // namespace zillians::language::tree::visitor
