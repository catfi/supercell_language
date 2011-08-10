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
 * @date Sep 1, 2009 sdk - Initial version created.
 */

#include "core/Prerequisite.h"
#include "compiler/tree/ASTNode.h"
#include "compiler/tree/ASTNodeFactory.h"
#include "compiler/tree/visitor/PrettyPrintVisitor.h"
#include <iostream>
#include <string>
#include <limits>

#define BOOST_TEST_MODULE ThorScriptTreeTest_PrettyPrintVisitorTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace zillians::compiler::tree;

BOOST_AUTO_TEST_SUITE( ThorScriptTreeTest_PrettyPrintVisitorTestSuite )

BOOST_AUTO_TEST_CASE( ThorScriptTreeTest_PrettyPrintVisitorTestCase1 )
{
	PrettyPrintVisitor printer;

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
							true,
							MemberScope::PUBLIC,
							StorageSpecifier::NONE);
					class_decl->appendFunction(some_member_function);
					{
					}

					VariableDecl* some_member_variable = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable"),
							true,
							MemberScope::PUBLIC,
							StorageSpecifier::STATIC
							);
					class_decl->appendVariable(some_member_variable);
				}
			}
		}
	}
	printer.visit(*program);
}

BOOST_AUTO_TEST_SUITE_END()
