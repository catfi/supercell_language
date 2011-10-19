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

#include "core/Prerequisite.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"
#include "language/stage/verifier/visitor/StaticTestVerificationStageVisitor.h"
#include "language/context/LogInfoContext.h"
#include "../ASTNodeSamples.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_StaticTestVerificationStageVisitorTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::language::tree;
using namespace zillians::language::tree::visitor;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_StaticTestVerificationStageVisitorTestTestSuite )

Program* createPassSample()
{
	Program* program = new Program();
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		program->root->addPackage(com_package);
		{
			Package* zillians_package = new Package(new SimpleIdentifier(L"zillians"));
			com_package->addPackage(zillians_package);
			{
				ClassDecl* class_decl = new ClassDecl(new SimpleIdentifier(L"some_class"));
				zillians_package->addObject(class_decl);
				{
					FunctionDecl* some_member_function = new FunctionDecl(
							new SimpleIdentifier(L"some_member_function"),
							NULL,
							true,
							false,
							Declaration::VisibilitySpecifier::PUBLIC,
							new Block());
					class_decl->addFunction(some_member_function);
					{
						Block* block = some_member_function->block;

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"a"), new TypeSpecifier(PrimitiveType::INT32), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"b"), new TypeSpecifier(PrimitiveType::INT32), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							// set error check annotation data
							Annotation* msgParams = new Annotation(NULL);
							msgParams->appendKeyValue(new SimpleIdentifier(L"id"), new PrimaryExpr(new StringLiteral(L"mCount")));
							msgParams->appendKeyValue(new SimpleIdentifier(L"type"), new PrimaryExpr(new StringLiteral(L"int")));
							Annotation* levelIdParams = new Annotation(NULL);
							levelIdParams->appendKeyValue(new SimpleIdentifier(L"level"), new PrimaryExpr(new StringLiteral(L"LEVEL_WARNING")));
							levelIdParams->appendKeyValue(new SimpleIdentifier(L"id"), new PrimaryExpr(new StringLiteral(L"EXAMPLE_UNDECLARED_VARIABLE")));
							levelIdParams->appendKeyValue(new SimpleIdentifier(L"parameters"), msgParams);

							Annotation* anno = new Annotation(new SimpleIdentifier(L"static_test"));
							anno->appendKeyValue(new SimpleIdentifier(L"expect_message"), levelIdParams);
							Annotations* annos = new Annotations();
							annos->appendAnnotation(anno);

							// set error message context data
							std::map<std::wstring, std::wstring> m;
							m[L"id"] = L"mCount";
							m[L"type"] = L"int";
							zillians::language::LogInfo errorContext(L"LEVEL_WARNING", L"EXAMPLE_UNDECLARED_VARIABLE", m);

							ExpressionStmt* stmt = new ExpressionStmt(new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"EXAMPLE_UNDECLARED_VARIABLE")), new PrimaryExpr(new SimpleIdentifier(L"b"))));
							stmt->setAnnotations(annos);
                            zillians::language::LogInfoContext::push_back(stmt, errorContext);

							block->appendObject(stmt);
						}
					}
				}
			}
		}
	}
	return program;
}

Program* createFailSample()
{
	Program* program = new Program();
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		program->root->addPackage(com_package);
		{
			Package* zillians_package = new Package(new SimpleIdentifier(L"zillians"));
			com_package->addPackage(zillians_package);
			{
				ClassDecl* class_decl = new ClassDecl(new SimpleIdentifier(L"some_class"));
				zillians_package->addObject(class_decl);
				{
					FunctionDecl* some_member_function = new FunctionDecl(
							new SimpleIdentifier(L"some_member_function"),
							NULL,
							true,
							false,
							Declaration::VisibilitySpecifier::PUBLIC,
							new Block);
					class_decl->addFunction(some_member_function);
					{
						Block* block = some_member_function->block;

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"a"), new TypeSpecifier(PrimitiveType::INT32), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"b"), new TypeSpecifier(PrimitiveType::INT32), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							// set error check annotation data
							Annotation* msgParams = new Annotation(NULL);
							msgParams->appendKeyValue(new SimpleIdentifier(L"id"), new PrimaryExpr(new StringLiteral(L"mCount")));
							msgParams->appendKeyValue(new SimpleIdentifier(L"type"), new PrimaryExpr(new StringLiteral(L"int")));
							msgParams->appendKeyValue(new SimpleIdentifier(L"extra_fail_key"), new PrimaryExpr(new StringLiteral(L"extra_fail_value")));
							Annotation* levelIdParams = new Annotation(NULL);
							levelIdParams->appendKeyValue(new SimpleIdentifier(L"level"), new PrimaryExpr(new StringLiteral(L"LEVEL_WARNING")));
							levelIdParams->appendKeyValue(new SimpleIdentifier(L"id"), new PrimaryExpr(new StringLiteral(L"EXAMPLE_UNDECLARED_VARIABLE")));
							levelIdParams->appendKeyValue(new SimpleIdentifier(L"parameters"), msgParams);

							Annotation* anno = new Annotation(new SimpleIdentifier(L"static_test"));
							anno->appendKeyValue(new SimpleIdentifier(L"expect_message"), levelIdParams);
							Annotations* annos = new Annotations();
							annos->appendAnnotation(anno);

							// set error message context data
							std::map<std::wstring, std::wstring> m;
							m[L"id"] = L"mCount";
							m[L"type"] = L"int";
                            zillians::language::LogInfo errorContext(L"LEVEL_WARNING", L"EXAMPLE_UNDECLARED_VARIABLE", m);

							ExpressionStmt* stmt = new ExpressionStmt(new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"EXAMPLE_UNDECLARED_VARIABLE")), new PrimaryExpr(new SimpleIdentifier(L"b"))));
							stmt->setAnnotations(annos);
                            zillians::language::LogInfoContext::push_back(stmt, errorContext);

							// set source info context
							int source_index = 0;
							zillians::language::stage::ModuleSourceInfoContext* module_info = new zillians::language::stage::ModuleSourceInfoContext();
							source_index = module_info->addSource("test.cpp");
							source_index = module_info->addSource("hello.cpp");
							zillians::language::stage::ModuleSourceInfoContext::set(program, module_info);
							zillians::language::stage::SourceInfoContext::set(stmt, new zillians::language::stage::SourceInfoContext(source_index, 32, 10) );

							block->appendObject(stmt);
						}
					}
				}
			}
		}
	}
	return program;
}

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_StaticTestVerificationStageVisitorTestCase1 )
{
	// prepare module info for debug purpose
	using namespace zillians::language;
    setParserContext(new ParserContext());

	stage::ModuleSourceInfoContext* module_info = new stage::ModuleSourceInfoContext();
	int source_index = 0;
	source_index = module_info->addSource("test.cpp");
	source_index = module_info->addSource("hello.cpp");

	zillians::language::stage::visitor::StaticTestVerificationStageVisitor checker;

	Program* okProgram = createPassSample();
	checker.programNode = okProgram;
    getParserContext().program = okProgram;
	stage::ModuleSourceInfoContext::set(okProgram, module_info);
	checker.check(*okProgram);
	BOOST_CHECK(checker.isAllMatch());

	Program* failProgram = createFailSample();
	checker.programNode = failProgram;
    getParserContext().program = okProgram;
	stage::ModuleSourceInfoContext::set(failProgram, module_info);
	checker.check(*failProgram);
	BOOST_CHECK(!checker.isAllMatch());
}

BOOST_AUTO_TEST_SUITE_END()
