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
 * @date Aug 4, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_ASTNODEFACTORY_H_
#define ZILLIANS_LANGUAGE_TREE_ASTNODEFACTORY_H_

#include "core/Prerequisite.h"
#include "utility/Foreach.h"

#include "language/tree/ASTNode.h"

#include "language/tree/basic/Block.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/Literal.h"
#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/basic/FunctionType.h"

#include "language/tree/module/Program.h"
#include "language/tree/module/Package.h"
#include "language/tree/module/Import.h"

#include "language/tree/declaration/Declaration.h"
#include "language/tree/declaration/ClassDecl.h"
#include "language/tree/declaration/EnumDecl.h"
#include "language/tree/declaration/InterfaceDecl.h"
#include "language/tree/declaration/TypedefDecl.h"
#include "language/tree/declaration/FunctionDecl.h"
#include "language/tree/declaration/VariableDecl.h"

#include "language/tree/statement/Statement.h"
#include "language/tree/statement/DeclarativeStmt.h"
#include "language/tree/statement/ExpressionStmt.h"
#include "language/tree/statement/IterativeStmt.h"
#include "language/tree/statement/SelectionStmt.h"
#include "language/tree/statement/BranchStmt.h"

#include "language/tree/expression/Expression.h"
#include "language/tree/expression/PrimaryExpr.h"
#include "language/tree/expression/UnaryExpr.h"
#include "language/tree/expression/BinaryExpr.h"
#include "language/tree/expression/TernaryExpr.h"
#include "language/tree/expression/MemberExpr.h"
#include "language/tree/expression/CallExpr.h"
#include "language/tree/expression/CastExpr.h"

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODEFACTORY_H_ */
