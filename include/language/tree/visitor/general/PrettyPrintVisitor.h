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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_PRETTYPRINTVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_PRETTYPRINTVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"

#define INDENT_CHAR  L' '
#define INDENT_WIDTH 2

#define STREAM set_indent(std::wcout, depth)

namespace zillians { namespace language { namespace tree { namespace visitor {

struct PrettyPrintVisitor : Visitor<const ASTNode, void>
{
	PrettyPrintVisitor()
	{
		REGISTER_VISITABLE(printInvoker,
				// basic
				ASTNode,
				Annotations,
				Annotation,
				Block,
				Identifier,
					SimpleIdentifier,
					NestedIdentifier,
					TemplatedIdentifier,
				Literal,
					NumericLiteral,
					StringLiteral,
				TypeSpecifier,
				FunctionType,

				// module
				Program,
				Package,
				Import,

				// declaration
				Declaration,
					ClassDecl,
					EnumDecl,
					FunctionDecl,
					InterfaceDecl,
					VariableDecl,
					TypedefDecl,

				// statement
				Statement,
					DeclarativeStmt,
					ExpressionStmt,
					IterativeStmt,
						ForeachStmt,
						WhileStmt,
					SelectionStmt,
						IfElseStmt,
						SwitchStmt,
					BranchStmt,

				// expression
				Expression,
					PrimaryExpr,
					UnaryExpr,
					BinaryExpr,
					TernaryExpr,
					MemberExpr,
					CallExpr,
					CastExpr
				);

		depth = 0;
	}

	void increaseIdent()
	{
		++depth;
	}

	void decreaseIdent()
	{
		--depth;
	}

	//////////////////////////////////////////////////////////////////////
	/// Basic

	void print(const ASTNode& node)
	{
		STREAM << L"<unknown_node/>" << std::endl;
	}

	void print(const Block& node)
	{
		foreach(i, node.objects)
		{
			visit(**i);
		}
	}

	void print(const Annotations& node)
	{
		foreach(i, node.annotation_list)
		{
			visit(**i);
		}
	}

	void print(const Annotation& node)
	{
		STREAM << L"<annotation name=\"" << node.name->toString() << L"\">" << std::endl;
	}

	void print(const Identifier& node)
	{
		STREAM << L"<identifier data=\"" << node.toString() << L"\">" << std::endl;
	}

	void print(const NumericLiteral& node)
	{
		STREAM << L"<numeric_literal type=\"" << NumericLiteral::LiteralType::toString(node.type) << L"\">" << std::endl;
		{
			increaseIdent();
			switch(node.type)
			{
			case NumericLiteral::LiteralType::I8: STREAM << L"<i8 value=\"" << (int64)node.value.i8 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::I16: STREAM << L"<i16 value=\"" << (int64)node.value.i16 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::I32: STREAM << L"<i32 value=\"" << (int64)node.value.i32 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::I64: STREAM << L"<i64 value=\"" << (int64)node.value.i64 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::U8: STREAM << L"<u8 value=\"" << (uint64)node.value.i8 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::U16: STREAM << L"<u16 value=\"" << (uint64)node.value.i16 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::U32: STREAM << L"<u32 value=\"" << (uint64)node.value.i32 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::U64: STREAM << L"<u64 value=\"" << (uint64)node.value.i64 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::F32: STREAM << L"<f32 value=\"" << node.value.f32 << L"\"/>" << std::endl; break;
			case NumericLiteral::LiteralType::F64: STREAM << L"<f32 value=\"" << node.value.f64 << L"\"/>" << std::endl; break;
			}
			decreaseIdent();
		}
		STREAM << L"</numeric_literal>" << std::endl;
	}

	void print(const StringLiteral& node)
	{
		STREAM << L"<string_literal data=\"" << node.value << "\"/>" << std::endl;
	}

	//////////////////////////////////////////////////////////////////////
	/// Module

	void print(const Program& node)
	{
		STREAM << L"<program>" << std::endl;
		if(node.root)
		{
			increaseIdent();
			visit(*node.root);
			decreaseIdent();
		}
		STREAM << L"</program>" << std::endl;
	}

	void print(const Package& node)
	{
		STREAM << L"<package name=\"" << node.id->toString() << L"\">" << std::endl;
		{
			increaseIdent();
			foreach(i, node.children)
			{
				visit(**i);
			}
			decreaseIdent();
		}
		{
			increaseIdent();
			foreach(i, node.objects)
			{
				visit(**i);
			}
			decreaseIdent();
		}
		STREAM << L"</package>" << std::endl;
	}

	void print(const Import& node)
	{
		STREAM << L"<import ns=\"" << node.ns->toString() << L"\"/>" << std::endl;
	}

	void print(const TypeSpecifier& node)
	{
		STREAM << L"<type_specifier referred_type=\"" << TypeSpecifier::ReferredType::toString(node.type) << L"\">" << std::endl;
		{
			increaseIdent();
			switch(node.type)
			{
			case TypeSpecifier::ReferredType::CLASS_DECL: if(node.referred.class_decl) visit(*node.referred.class_decl); break;
			case TypeSpecifier::ReferredType::FUNCTION_DECL: if(node.referred.function_decl) visit(*node.referred.function_decl); break;
			case TypeSpecifier::ReferredType::ENUM_DECL: if(node.referred.enum_decl) visit(*node.referred.enum_decl); break;
			case TypeSpecifier::ReferredType::TYPEDEF_DECL: if(node.referred.typedef_decl) visit(*node.referred.typedef_decl); break;
			case TypeSpecifier::ReferredType::FUNCTION_TYPE: if(node.referred.function_type) visit(*node.referred.function_type); break;
			case TypeSpecifier::ReferredType::UNSPECIFIED: if(node.referred.unspecified) visit(*node.referred.unspecified); break;
			case TypeSpecifier::ReferredType::PRIMITIVE:
			{
				STREAM << L"<primitive type=\"" << TypeSpecifier::PrimitiveType::toString(node.referred.primitive) << L"\"/>" << std::endl;
				break;
			}
			}
			decreaseIdent();
		}
		STREAM << L"</type_specifier>" << std::endl;
	}

	void print(const FunctionType& node)
	{
		STREAM << L"<function_type>" << std::endl;
		{
			int index = 0;
			increaseIdent();
			foreach(i, node.templated_parameters)
			{
				STREAM << L"<templated_parameter index=\"" << index << "\">" << std::endl;
				{
					increaseIdent();
					visit(**i);
					decreaseIdent();
				}
				STREAM << L"</templated_parameter>" << std::endl;
				++index;
			}
			index = 0;
			foreach(i, node.argument_types)
			{
				STREAM << L"<argument_type index=\"" << index << "\">" << std::endl;
				{
					increaseIdent();
					visit(**i);
					decreaseIdent();
				}
				STREAM << L"</argument_type>" << std::endl;
				++index;
			}
			decreaseIdent();
		}
		STREAM << L"</function_type>" << std::endl;
	}

	//////////////////////////////////////////////////////////////////////
	/// Declaration

	void print(const ClassDecl& node)
	{
		STREAM << L"<class_decl name=\"" << node.name->toString() << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			foreach(i, node.member_functions)
			{
				visit(**i);
			}
			decreaseIdent();
		}
		{
			increaseIdent();
			foreach(i, node.member_variables)
			{
				visit(**i);
			}
			decreaseIdent();
		}
		STREAM << L"</class_decl>" << std::endl;
	}

	void print(const EnumDecl& node)
	{
		STREAM << L"<enum_decl name=\"" << node.name->toString() << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			foreach(i, node.enumeration_list)
			{
				STREAM << L"<enumeration key=\"" << i->first->toString() << "\">" << std::endl;
				if(i->second)
				{
					increaseIdent();
					STREAM << L"<value>" << std::endl;
					visit(*i->second);
					STREAM << L"</value>" << std::endl;
					decreaseIdent();
				}
				STREAM << L"</enumeration>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</enum_decl>" << std::endl;
	}

	void print(const FunctionDecl& node)
	{
		STREAM << L"<function_decl " <<
				L"name=\"" << ((node.name) ? node.name->toString() : L"<unspecified-null>") << L"\" " <<
				L"type=\"" << decodeType(node.type) << L"\" " <<
				L"is_member=\"" << (node.is_member ? L"true" : L"false") << L"\" " <<
				L"visibility=\"" << Declaration::VisibilitySpecifier::toString(node.visibility) << L"\" " <<
				L"storage=\"" << Declaration::StorageSpecifier::toString(node.storage) << L"\"" <<
				L">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				foreach(i, node.parameters)
				{
					STREAM << L"<parameters name=\"" << i->first->toString() << "\">" << std::endl;
					{
						increaseIdent();
						visit(*i->second);
						decreaseIdent();
					}
					STREAM << L"</parameters>" << std::endl;
				}
			}
			decreaseIdent();

			increaseIdent();
			if(node.block)
			{
				STREAM << L"<block>" << std::endl;
				increaseIdent();
				visit(*node.block);
				decreaseIdent();
				STREAM << L"</blocl>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</function_decl>" << std::endl;
	}

	void print(const InterfaceDecl& node)
	{
		STREAM << L"<interface_decl name=\"" << node.name->toString() << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			foreach(i, node.member_functions)
			{
				visit(**i);
			}
			decreaseIdent();
		}
		STREAM << L"</interface_decl>" << std::endl;
	}

	void print(const VariableDecl& node)
	{
		STREAM << L"<variable_decl " <<
				L"name=\"" << node.name->toString() << L"\" " <<
				L"type=\"" << decodeType(node.type) << L"\" "
				L"is_member=\"" << (node.is_member ? L"true" : L"false") << L"\" " <<
				L"visibility=\"" << Declaration::VisibilitySpecifier::toString(node.visibility) << L"\" " <<
				L"storage=\"" << Declaration::StorageSpecifier::toString(node.storage) << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		if(node.initializer)
		{
			STREAM << L"<initializer>" << std::endl;
			{
				increaseIdent();
				visit(*node.initializer);
				decreaseIdent();
			}
			STREAM << L"</initializer>" << std::endl;
		}

		STREAM << L"</variable_decl>" << std::endl;
	}

	void print(const TypedefDecl& node)
	{
		STREAM << L"<typedef_decl from=\"" << decodeType(node.from) << L"\" to=\"" << node.to->toString() << L"\"/>" << std::endl;
	}

	//////////////////////////////////////////////////////////////////////
	/// Statement

	void print(const DeclarativeStmt& node)
	{
		STREAM << L"<declarative_stmt>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			visit(*node.declaration);
			decreaseIdent();
		}
		STREAM << L"</declarative_stmt>" << std::endl;
	}

	void print(const ExpressionStmt& node)
	{
		STREAM << L"<expression_stmt>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			visit(*node.expr);
			decreaseIdent();
		}
		STREAM << L"</expression_stmt>" << std::endl;
	}

	void print(const ForeachStmt& node)
	{
		STREAM << L"<foreach_stmt>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			if(node.iterator)
			{
				STREAM << L"<iterator>" << std::endl;
				{
					increaseIdent();
					visit(*node.iterator);
					decreaseIdent();
				}
				STREAM << L"</iterator>" << std::endl;
			}
			else
			{
				STREAM << L"<invalid_iterator/>" << std::endl;
			}
			if(node.range)
			{
				STREAM << L"<range>" << std::endl;
				{
					increaseIdent();
					visit(*node.range);
					decreaseIdent();
				}
				STREAM << L"</range>" << std::endl;
			}
			else
			{
				STREAM << L"<invalid_range/>" << std::endl;
			}
			if(node.block)
			{
				STREAM << L"<block>" << std::endl;
				{
					increaseIdent();
					visit(*node.block);
					decreaseIdent();
				}
				STREAM << L"</block>" << std::endl;
			}
			else
			{
				STREAM << L"<null_block/>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</foreach_stmt>" << std::endl;
	}

	void print(const WhileStmt& node)
	{
		STREAM << L"<while_stmt type=\"" << WhileStmt::Style::toString(node.style) << "\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			if(node.cond)
			{
				STREAM << L"<condition>" << std::endl;
				{
					increaseIdent();
					visit(*node.cond);
					decreaseIdent();
				}
				STREAM << L"</condition>" << std::endl;
			}
			else
			{
				STREAM << L"<invalid_condition/>" << std::endl;
			}
			if(node.block)
			{
				STREAM << L"<block>" << std::endl;
				{
					increaseIdent();
					visit(*node.block);
					decreaseIdent();
				}
				STREAM << L"</block>" << std::endl;
			}
			else
			{
				STREAM << L"<null_block/>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</while_stmt>" << std::endl;
	}

	void print(const IfElseStmt& node)
	{
		STREAM << L"<ifelse_stmt>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<if>" << std::endl;
				{
					increaseIdent();
					{
						STREAM << L"<condition>" << std::endl;
						{
							increaseIdent();
							if(node.if_branch.cond) visit(*node.if_branch.cond);
							decreaseIdent();
						}
						STREAM << L"</condition>" << std::endl;
					}
					{
						STREAM << L"<block>" << std::endl;
						{
							increaseIdent();
							if(node.if_branch.block) visit(*node.if_branch.block);
							decreaseIdent();
						}
						STREAM << L"</block>" << std::endl;
					}
					decreaseIdent();
				}
				STREAM << L"</if>" << std::endl;
			}
			int c = 0;
			foreach(i, node.elseif_branches)
			{
				STREAM << L"<elseif index=\"" << c << L"\">" << std::endl;
				{
					increaseIdent();
					{
						STREAM << L"<condition>" << std::endl;
						{
							increaseIdent();
							if(i->cond) visit(*i->cond);
							decreaseIdent();
						}
						STREAM << L"</condition>" << std::endl;
					}
					{
						STREAM << L"<block>" << std::endl;
						{
							increaseIdent();
							if(i->block) visit(*i->block);
							decreaseIdent();
						}
						STREAM << L"</block>" << std::endl;
					}
					decreaseIdent();
				}
				STREAM << L"</elseif>" << std::endl;
				++c;
			}
			{
				STREAM << L"<else>" << std::endl;
				{
					increaseIdent();
					{
						STREAM << L"<block>" << std::endl;
						{
							increaseIdent();
							if(node.else_block) visit(*node.else_block);
							decreaseIdent();
						}
						STREAM << L"</block>" << std::endl;
					}
					decreaseIdent();
				}
				STREAM << L"</else>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</ifelse_stmt>" << std::endl;
	}

	void print(const SwitchStmt& node)
	{
		STREAM << L"<switch_stmt>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<node>" << std::endl;
				{
					increaseIdent();
					if(node.node) visit(*node.node);
					decreaseIdent();
				}
				STREAM << L"</node>" << std::endl;
			}
			int c = 0;
			foreach(i, node.cases)
			{
				STREAM << L"<case index=\"" << c << "\">" << std::endl;
				{
					increaseIdent();
					{
						STREAM << L"<condition>" << std::endl;
						{
							increaseIdent();
							if(i->cond) visit(*i->cond);
							decreaseIdent();
						}
						STREAM << L"</condition>" << std::endl;
					}
					{
						STREAM << L"<block>" << std::endl;
						{
							increaseIdent();
							if(i->block) visit(*i->block);
							decreaseIdent();
						}
						STREAM << L"</block>" << std::endl;
					}
					decreaseIdent();
				}
				STREAM << L"</case>" << std::endl;
				++c;
			}
			decreaseIdent();
		}
		STREAM << L"</switch_stmt>" << std::endl;
	}

	void print(const BranchStmt& node)
	{
		if(node.opcode == BranchStmt::OpCode::RETURN)
		{
			STREAM << L"<branch_stmt opcode=\"" << BranchStmt::OpCode::toString(node.opcode) << L"\"/>" << std::endl;
			{
				printAnnotation(node.annotations);
			}
			{
				increaseIdent();
				if(node.result)
				{
					STREAM << L"<block>" << std::endl;
					{
						increaseIdent();
						visit(*node.result);
						decreaseIdent();
					}
					STREAM << L"</block>" << std::endl;
				}
				else
				{
					STREAM << L"<null_block/>" << std::endl;
				}
				decreaseIdent();
			}
			STREAM << L"</branch_stmt>" << std::endl;
		}
		else
		{
			STREAM << L"<branch_stmt opcode=\"" << BranchStmt::OpCode::toString(node.opcode) << L"\">" << std::endl;
			{
				printAnnotation(node.annotations);
			}
			STREAM << L"</branch_stmt>" << std::endl;
		}
	}

	//////////////////////////////////////////////////////////////////////
	/// Expression

	void print(const PrimaryExpr& node)
	{
		STREAM << L"<primary_expr type=\"" << PrimaryExpr::Catagory::toString(node.catagory) << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			switch(node.catagory)
			{
			case PrimaryExpr::Catagory::IDENTIFIER: if(node.value.identifier) visit(*node.value.identifier); break;
			case PrimaryExpr::Catagory::LITERAL: if(node.value.literal) visit(*node.value.literal); break;
			case PrimaryExpr::Catagory::LAMBDA: if(node.value.lambda) visit(*node.value.lambda); break;
			}
			decreaseIdent();
		}
		STREAM << L"</primary_expr>" << std::endl;
	}

	void print(const UnaryExpr& node)
	{
		STREAM << L"<unary_expr opcode=\"" << UnaryExpr::OpCode::toString(node.opcode) << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			visit(*node.node);
			decreaseIdent();
		}
		STREAM << L"</unary_expr>" << std::endl;
	}

	void print(const BinaryExpr& node)
	{
		STREAM << L"<binary_expr opcode=\"" << BinaryExpr::OpCode::toString(node.opcode) << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<left>" << std::endl;
				{
					increaseIdent();
					visit(*node.left);
					decreaseIdent();
				}
				STREAM << L"</left>" << std::endl;
			}
			{
				STREAM << L"<right>" << std::endl;
				{
					increaseIdent();
					visit(*node.right);
					decreaseIdent();
				}
				STREAM << L"</right>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</binary_expr>" << std::endl;
	}

	void print(const TernaryExpr& node)
	{
		STREAM << L"<ternary_expr>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<condition>" << std::endl;
				{
					increaseIdent();
					visit(*node.cond);
					decreaseIdent();
				}
				STREAM << L"</condition>" << std::endl;
			}
			{
				STREAM << L"<true_node>" << std::endl;
				{
					increaseIdent();
					visit(*node.true_node);
					decreaseIdent();
				}
				STREAM << L"</true_node>" << std::endl;
			}
			{
				STREAM << L"<false_node>" << std::endl;
				{
					increaseIdent();
					visit(*node.false_node);
					decreaseIdent();
				}
				STREAM << L"</false_node>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</ternary_expr>" << std::endl;
	}

	void print(const MemberExpr& node)
	{
		STREAM << L"<member_expr member=\"" << node.member->toString() << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<left_hand_side>" << std::endl;
				{
					increaseIdent();
					visit(*node.node);
					decreaseIdent();
				}
				STREAM << L"</left_hand_side>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</member_expr>" << std::endl;
	}

	void print(const CallExpr& node)
	{
		STREAM << L"<call_expr>" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<left_hand_side>" << std::endl;
				{
					increaseIdent();
					visit(*node.node);
					decreaseIdent();
				}
				STREAM << L"</left_hand_side>" << std::endl;
			}
			{
				STREAM << L"<parameters>" << std::endl;
				{
					increaseIdent();
					foreach(i, node.parameters)
					{
						visit(**i);
					}
					decreaseIdent();
				}
				STREAM << L"</parameters>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</call_expr>" << std::endl;
	}

	void print(const CastExpr& node)
	{
		STREAM << L"<cast_expr type=\"" << decodeType(node.type) << L"\">" << std::endl;
		{
			printAnnotation(node.annotations);
		}
		{
			increaseIdent();
			{
				STREAM << L"<node>" << std::endl;
				{
					increaseIdent();
					visit(*node.node);
					decreaseIdent();
				}
				STREAM << L"</node>" << std::endl;
			}
			decreaseIdent();
		}
		STREAM << L"</cast_expr>" << std::endl;
	}

private:
	void printAnnotation(Annotations* annotations)
	{
		if(annotations && annotations->annotation_list.size() > 0)
		{
			increaseIdent();
			{
				STREAM << L"<annotations>" << std::endl;
				{
					increaseIdent();
					foreach(i, annotations->annotation_list)
					{
						STREAM << L"<annotation name=\"" << (*i)->name->toString() << L"\">" << std::endl;
						{
							increaseIdent();
							foreach(j, (*i)->attribute_list)
							{
								STREAM << L"<key>" << std::endl;
								{
									increaseIdent();
									visit(*j->first);
									decreaseIdent();
								}
								STREAM << L"</key>" << std::endl;

								STREAM << L"<value>" << std::endl;
								{
									increaseIdent();
									visit(*j->second);
									decreaseIdent();
								}
								STREAM << L"</value>" << std::endl;
							}
							decreaseIdent();
						}
						STREAM << L"</annotation>" << std::endl;
					}
					decreaseIdent();
				}
				STREAM << L"</annotations>" << std::endl;
			}
			decreaseIdent();
		}
	}

	static std::wstring decodeType(ASTNode* type)
	{
		if(!type)
			return L"<unspecified>";

		if(!isa<TypeSpecifier>(type))
		{
			return L"<invalid>";
		}
		else
		{
			//return L"others";
			TypeSpecifier* t = cast<TypeSpecifier>(type);
			switch(t->type)
			{
			case TypeSpecifier::ReferredType::CLASS_DECL: return t->referred.class_decl->name->toString();
			case TypeSpecifier::ReferredType::INTERFACE_DECL: return t->referred.interface_decl->name->toString();
			case TypeSpecifier::ReferredType::FUNCTION_DECL: return t->referred.function_decl->name->toString();
			case TypeSpecifier::ReferredType::ENUM_DECL: return t->referred.enum_decl->name->toString();
			case TypeSpecifier::ReferredType::TYPEDEF_DECL: return t->referred.typedef_decl->to->toString();
			case TypeSpecifier::ReferredType::PRIMITIVE: return TypeSpecifier::PrimitiveType::toString(t->referred.primitive);
			case TypeSpecifier::ReferredType::UNSPECIFIED: return t->referred.unspecified->toString();
			case TypeSpecifier::ReferredType::FUNCTION_TYPE: return decodeFunctionType(t->referred.function_type);
			}
		}
		return L"error";
	}

	static std::wstring decodeFunctionType(FunctionType* type)
	{
		std::wstringstream ss;

		ss << L"function";
		if(type->templated_parameters.size() > 0)
		{
			ss << L"<";
			foreach(i, type->templated_parameters)
				ss << (*i)->toString() << ((i != type->templated_parameters.end()) ? L", " : L"");
			ss << L">";
		}
		ss << "(";
		foreach(i, type->argument_types)
			ss << decodeType(*i) << ((i != type->argument_types.end()) ? L", " : L"");
		ss << ")";

		return ss.str();
	}

	static std::basic_ostream<wchar_t>& set_indent(std::basic_ostream<wchar_t> &ss, size_t depth)
	{
		return ss << std::setfill(INDENT_CHAR) << std::setw(INDENT_WIDTH*depth) << L"";
	}

	CREATE_INVOKER(printInvoker, print);

	int depth;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_PRETTYPRINTVISITOR_H_ */
