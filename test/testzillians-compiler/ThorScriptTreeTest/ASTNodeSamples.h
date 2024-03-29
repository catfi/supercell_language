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
    Tangle* tangle = new Tangle();
	Source* source = new Source("test-program-1", false);
    tangle->addSource(new SimpleIdentifier(L"package-id"), source);
	return tangle;
}

ASTNode* createSample2()
{
    Tangle* tangle = new Tangle();
	Source* source = new Source("test-program-2", false);
    tangle->addSource(new SimpleIdentifier(L"package-id"), source);
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		source->root->addPackage(com_package);
		{
			Package* supercell_package = new Package(new SimpleIdentifier(L"supercell"));
			com_package->addPackage(supercell_package);

			Package* zillians_package = new Package(new SimpleIdentifier(L"zillians"));
			com_package->addPackage(zillians_package);
		}
	}
	return tangle;
}

ASTNode* createSample3()
{
    Tangle* tangle = new Tangle();
	Source* source = new Source("test-program-3", false);
    tangle->addSource(new SimpleIdentifier(L"package-id"), source);
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		source->root->addPackage(com_package);
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
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"a"), new TypeSpecifier(PrimitiveType::INT32_TYPE), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							DeclarativeStmt* stmt = new DeclarativeStmt(new VariableDecl(new SimpleIdentifier(L"b"), new TypeSpecifier(PrimitiveType::INT32_TYPE), false, false, false, Declaration::VisibilitySpecifier::DEFAULT));
							block->appendObject(stmt);
						}

						{
							ExpressionStmt* stmt = new ExpressionStmt(new BinaryExpr(BinaryExpr::OpCode::ASSIGN, new PrimaryExpr(new SimpleIdentifier(L"a")), new PrimaryExpr(new SimpleIdentifier(L"b"))));
							block->appendObject(stmt);
						}
					}

					VariableDecl* some_member_variable1 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable1"),
							new TypeSpecifier(PrimitiveType::FLOAT64_TYPE),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable1);

					VariableDecl* some_member_variable2 = new VariableDecl(
							new SimpleIdentifier(L"some_member_variable2"),
							new TypeSpecifier(PrimitiveType::FLOAT32_TYPE),
							true, false, false,
							Declaration::VisibilitySpecifier::PUBLIC);
					class_decl->addVariable(some_member_variable2);
				}
			}
		}
	}

    return tangle;
}

ASTNode* createSample4()
{
    Tangle* tangle = new Tangle();
	Source* source = new Source("test-program-4", false);
    tangle->addSource(new SimpleIdentifier(L"package-id"), source);
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		source->root->addPackage(com_package);
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
							Declaration::VisibilitySpecifier::PUBLIC,
							new Block());
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

	return tangle;
}

// class with member function template
ASTNode* createSample5()
{
    Tangle* tangle = new Tangle();
	Source* source = new Source("test-program-5", false);
    tangle->addSource(new SimpleIdentifier(L"package-id"), source);
	{
		Package* com_package = new Package(new SimpleIdentifier(L"com"));
		source->root->addPackage(com_package);
		{
			Package* zillians_package = new Package(new SimpleIdentifier(L"zillians"));
			com_package->addPackage(zillians_package);
			{
				ClassDecl* class_decl = new ClassDecl(new SimpleIdentifier(L"some_class"));
				zillians_package->addObject(class_decl);
				{
                    // create annotations
                    Annotations* annotations = new Annotations();
                    annotations->appendAnnotation(new Annotation(new SimpleIdentifier(L"server")));
                    // normal function
					FunctionDecl* some_member_function = new FunctionDecl(
							new SimpleIdentifier(L"some_member_function"),
							NULL,
							true,
							false,
							Declaration::VisibilitySpecifier::PUBLIC,
							new Block());
					class_decl->addFunction(some_member_function);
                    some_member_function->setAnnotations(annotations);

                    // formal function template
					FunctionDecl* template_function = new FunctionDecl(
                            new TemplatedIdentifier(TemplatedIdentifier::Usage::FORMAL_PARAMETER, new SimpleIdentifier(L"some_member_function")),
                            NULL,
                            true,
                            false,
                            Declaration::VisibilitySpecifier::PUBLIC,
                            new Block());
					class_decl->addFunction(template_function);

                    // specialized function template
					FunctionDecl* specialized_function = new FunctionDecl(
                            new TemplatedIdentifier(TemplatedIdentifier::Usage::ACTUAL_ARGUMENT, new SimpleIdentifier(L"some_member_function")),
                            NULL,
                            true,
                            false,
                            Declaration::VisibilitySpecifier::PUBLIC,
                            new Block());
					class_decl->addFunction(specialized_function);
				}
			}
		}
	}

	return tangle;
}

#endif /* ASTNODESAMPLES_H_ */
