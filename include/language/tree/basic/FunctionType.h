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

#ifndef ZILLIANS_LANGUAGE_TREE_FUNCTIONTYPE_H_
#define ZILLIANS_LANGUAGE_TREE_FUNCTIONTYPE_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"

namespace zillians { namespace language { namespace tree {

// forward declaration of TypeSpecifier to avoid mutual header inclusion
struct TypeSpecifier;// : public ASTNode { };

struct FunctionType: public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(FunctionType, (FunctionType)(ASTNode));

	// TODO implement the conversion to convert argument types from identifier to placeholders
//	struct Placeholders
//	{
//		enum type {
//			T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20
//		};
//
//		static const wchar_t* toString(type t)
//		{
//			switch(t)
//			{
//			case T0: return L"T0";
//			case T1: return L"T1";
//			case T2: return L"T2";
//			case T3: return L"T3";
//			case T4: return L"T4";
//			case T5: return L"T5";
//			case T6: return L"T6";
//			case T7: return L"T7";
//			case T8: return L"T8";
//			case T9: return L"T9";
//			case T10: return L"T10";
//			case T11: return L"T11";
//			case T12: return L"T12";
//			case T13: return L"T13";
//			case T14: return L"T14";
//			case T15: return L"T15";
//			case T16: return L"T16";
//			case T17: return L"T17";
//			case T18: return L"T18";
//			case T19: return L"T19";
//			}
//		}
//	};

	FunctionType() : return_type(NULL)
	{ }

	void appendTemplateParameter(Identifier* parameter)
	{
		//parameter->parent = this;
		templated_parameters.push_back(parameter);
	}

	void appendParameterType(TypeSpecifier* type)
	{
		//type->parent = this;
		argument_types.push_back(type);
	}

	void setReturnType(TypeSpecifier* type)
	{
		return_type = type;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(templated_parameters)
		COMPARE_MEMBER(argument_types)
		COMPARE_MEMBER(return_type)
		END_COMPARE()
    }

	std::vector<Identifier*> templated_parameters;
	std::vector<TypeSpecifier*> argument_types;
	TypeSpecifier* return_type;
};

} } }


#endif /* ZILLIANS_LANGUAGE_TREE_FUNCTIONTYPE_H_ */
