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
 * @date Aug 12, 2011 sdk - Initial version created.
 */

#ifndef ASTNODESAMPLES_H_
#define ASTNODESAMPLES_H_

#include "core/Prerequisite.h"
#include "language/tree/ASTNode.h"
#include "language/tree/ASTNodeFactory.h"

using namespace zillians;
using namespace zillians::language::tree;

ASTNode* createSample1()
{
	Program* program = new Program();
	return program;
}

ASTNode* createSample2()
{
	Program* program = new Program();
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		program->root->addPackage(com_package);
		{
			Package* supercell_package = new Package(new SimpleIdentifier(L"supercell"));
			com_package->addPackage(supercell_package);

			Package* zillians_package = new Package(new SimpleIdentifier(L"zillians"));
			com_package->addPackage(zillians_package);
		}
	}
	return program;
}

ASTNode* createSample3()
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
							Declaration::VisibilitySpecifier::PUBLIC);
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
							ExpressionStmt* stmt = new ExpressionStmt(new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"a")), new PrimaryExpr(new SimpleIdentifier(L"b"))));
							block->appendObject(stmt);
						}
					}

					VariableDecl* some_member_variable1 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable1"),
							new TypeSpecifier(PrimitiveType::FLOAT64),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable1);

					VariableDecl* some_member_variable2 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable2"),
							new TypeSpecifier(PrimitiveType::FLOAT32),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable2);
				}
			}
		}
	}

	return program;
}

ASTNode* createSample4()
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
							true, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addFunction(some_member_function);
					{
						Block* block = some_member_function->block;

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"a"), new TypeSpecifier(new SimpleIdentifier(L"some_class")), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"b"), new TypeSpecifier(new SimpleIdentifier(L"some_class")), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							ExpressionStmt* stmt = new ExpressionStmt(new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"a")), new PrimaryExpr(new SimpleIdentifier(L"b"))));
							block->appendObject(stmt);
						}
					}

					VariableDecl* some_member_variable1 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable1"),
							new TypeSpecifier(new SimpleIdentifier(L"some_class")),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable1);

					VariableDecl* some_member_variable2 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable2"),
							new TypeSpecifier(new SimpleIdentifier(L"some_class")),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable2);
				}
			}
		}
	}

	return program;
}

ASTNode* createSample5()
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
							true, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addFunction(some_member_function);
					{
						Block* block = some_member_function->block;

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"a"), new TypeSpecifier(class_decl), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"b"), new TypeSpecifier(class_decl), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							ExpressionStmt* stmt = new ExpressionStmt(new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"a")), new PrimaryExpr(new SimpleIdentifier(L"b"))));
							block->appendObject(stmt);
						}
					}

					VariableDecl* some_member_variable1 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable1"),
							new TypeSpecifier(class_decl),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable1);

					VariableDecl* some_member_variable2 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable2"),
							new TypeSpecifier(class_decl),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable2);
				}
			}
		}
	}

	return program;
}

#endif /* ASTNODESAMPLES_H_ */
