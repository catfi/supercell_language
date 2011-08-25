/**
 * Zillians MMO
 * Copyright (C) 2007-2011 Zillians.com, Inc.
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

#ifndef NODETYPENAMEVISITOR_H_
#define NODETYPENAMEVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/visitor/general/GenericVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NodeTypeNameVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(nameInvoker, name)

	NodeTypeNameVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(nameInvoker)
	}

	void name(ASTNode& node)			{ class_name = L"unknown";			}
	void name(Annotations& node)		{ class_name = L"Annotations";		}
	void name(Annotation& node)			{ class_name = L"Annotation";		}
	void name(Block& node)				{ class_name = L"Block";			}
	void name(Identifier& node)			{ class_name = L"Identifier";		}
	void name(SimpleIdentifier& node)	{ class_name = L"SimpleIdentifier";	}
	void name(NestedIdentifier& node)	{ class_name = L"NestedIdentifier";	}
	void name(TemplatedIdentifier& node){ class_name = L"TemplatedIdentifier";	}
	void name(Literal& node)			{ class_name = L"Literal";			}
	void name(NumericLiteral& node)		{ class_name = L"NumericLiteral";	}
	void name(StringLiteral& node)		{ class_name = L"StringLiteral";	}
	void name(ObjectLiteral& node)		{ class_name = L"ObjectLiteral";	}
	void name(TypeSpecifier& node)		{ class_name = L"TypeSpecifier";	}
	void name(FunctionType& node)		{ class_name = L"FunctionType";		}
	void name(Program& node)			{ class_name = L"Program";			}
	void name(Package& node)			{ class_name = L"Package";			}
	void name(Import& node)				{ class_name = L"Import";			}
	void name(Declaration& node)		{ class_name = L"Declaration";		}
	void name(ClassDecl& node)			{ class_name = L"ClassDecl";		}
	void name(EnumDecl& node)			{ class_name = L"EnumDecl";			}
	void name(FunctionDecl& node)		{ class_name = L"FunctionDecl";		}
	void name(InterfaceDecl& node)		{ class_name = L"InterfaceDecl";	}
	void name(VariableDecl& node)		{ class_name = L"VariableDecl";		}
	void name(TypedefDecl& node)		{ class_name = L"TypedefDecl";		}
	void name(Statement& node)			{ class_name = L"Statement";		}
	void name(DeclarativeStmt& node)	{ class_name = L"DeclarativeStmt";	}
	void name(ExpressionStmt& node)		{ class_name = L"ExpressionStmt";	}
	void name(IterativeStmt& node)		{ class_name = L"IterativeStmt";	}
	void name(ForeachStmt& node)		{ class_name = L"ForeachStmt";		}
	void name(WhileStmt& node)			{ class_name = L"WhileStmt";		}
	void name(SelectionStmt& node)		{ class_name = L"SelectionStmt";	}
	void name(IfElseStmt& node)			{ class_name = L"IfElseStmt";		}
	void name(SwitchStmt& node)			{ class_name = L"SwitchStmt";		}
	void name(BranchStmt& node)			{ class_name = L"BranchStmt";		}
	void name(Expression& node)			{ class_name = L"Expression";		}
	void name(PrimaryExpr& node)		{ class_name = L"PrimaryExpr";		}
	void name(UnaryExpr& node)			{ class_name = L"UnaryExpr";		}
	void name(BinaryExpr& node)			{ class_name = L"BinaryExpr";		}
	void name(TernaryExpr& node)		{ class_name = L"TernaryExpr";		}
	void name(MemberExpr& node)			{ class_name = L"MemberExpr";		}
	void name(CallExpr& node)			{ class_name = L"CallExpr";			}
	void name(CastExpr& node)			{ class_name = L"CastExpr";			}

	std::wstring class_name;
};

} } } }

#endif /* NODETYPENAMEVISITOR_H_ */
