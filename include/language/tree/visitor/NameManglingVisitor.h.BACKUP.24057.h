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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_

#include "core/Prerequisite.h"
#include "utility/UnicodeUtil.h"
#include "core/Visitor.h"
#include "language/tree/visitor/GenericVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NameManglingVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(mangleInvoker, mangle)

	NameManglingVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mangleInvoker)
	}

	void mangle(ASTNode& node)
	{
		if(node.parent) visit(*node.parent);
	}

	void mangle(Identifier& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		stream << encode(node.toString());
	}

	void mangle(Package& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		if(node.id->isEmpty())
			//stream << "root";
			stream << "_";
		else
			stream << encode(node.id->toString());
	}

	void mangle(Block& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		stream << "_B_";
	}

	void mangle(TypeSpecifier& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		switch(node.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
			visit(*node.referred.function_type);
			break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
			stream << PrimitiveType::toString(node.referred.primitive);
			break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:
			stream << encode(node.referred.unspecified->toString());
			break;
		}
	}

	void mangle(ClassDecl& node)
	{
		if(node.parent) visit(*node.parent);

<<<<<<< Updated upstream
		if(stream.str().length() > 0)
			stream << "_";
=======
		if(node.argument_types.empty())
			stream << "v"; // empty param-list equivalent to "void" param type
		else
		{
			mInsideParamList = true;
			foreach(i, node.argument_types)
			{
				if((*i)->type == TypeSpecifier::ReferredType::PRIMITIVE)
					resolveAndVisit(*i);
				else
				{
					int type_index = addToRepeatTypeSet(*i);
					if(type_index != -1)
						writeRepeatTypeAlias(type_index+1);
					else
						resolveAndVisit(*i);
				}
			}
			mInsideParamList = false;
		}
>>>>>>> Stashed changes

		stream << encode(node.name->toString());
	}

	void mangle(InterfaceDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		stream << encode(node.name->toString());
	}

	void mangle(EnumDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		stream << encode(node.name->toString());
	}

	void mangle(FunctionDecl& node)
	{
<<<<<<< Updated upstream
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << "_";

		stream << encode(node.name->toString());
=======
		auto p = node.parameters.begin();
		if(node.is_member)
		{
			BOOST_ASSERT((node.parameters.size() >= 1) && "method must have 1st parameter \"this\"");
			addToRepeatTypeSet((*p)->type);
			p++;
		}

		uptraceAndAppendName(&node);
		clearRepeatTypeSet();
		if(p == node.parameters.end())
			stream << "v"; // empty param-list equivalent to "void" param type
		else
		{
			mInsideParamList = true;
			for(; p != node.parameters.end(); p++)
			{
				if((*p)->type->type == TypeSpecifier::ReferredType::PRIMITIVE)
					resolveAndVisit((*p)->type);
				else
				{
					int type_index = addToRepeatTypeSet((*p)->type);
					if(type_index != -1)
						writeRepeatTypeAlias(type_index+1);
					else
						resolveAndVisit((*p)->type);
				}
			}
			mInsideParamList = false;
		}
>>>>>>> Stashed changes
	}

	void reset()
	{
		stream.str("");
	}

	const std::string& encode(const std::wstring& ucs4)
	{
		ucs4_to_utf8_temp.clear();
		utf8_to_llvm_temp.clear();

		// first we should covert UCS-4 to UTF-8
		ucs4_to_utf8(ucs4, ucs4_to_utf8_temp);

		// because LLVM only accept identifier of the form: '[%@][a-zA-Z$._][a-zA-Z$._0-9]*'
		// we have to convert illegal identifier into legal one
		for(std::string::const_iterator i = ucs4_to_utf8_temp.begin(), e = ucs4_to_utf8_temp.end(); i != e; ++i)
		{
			char c = *i;
			if(i == ucs4_to_utf8_temp.begin())
			{
				if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c == '.') )
				{
					utf8_to_llvm_temp.push_back(c);
				}
				else
				{
					utf8_to_llvm_temp.push_back('$');
					utf8_to_llvm_temp.append(toAsciiNumber(c));
					utf8_to_llvm_temp.push_back('$');
				}
			}
			else
			{
				if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '.') )
				{
					utf8_to_llvm_temp.push_back(c);
				}
				else
				{
					utf8_to_llvm_temp.push_back('$');
					utf8_to_llvm_temp.append(toAsciiNumber(c));
					utf8_to_llvm_temp.push_back('$');
				}
			}
		}

		return utf8_to_llvm_temp;
	}

	inline const char* toAsciiNumber(char c)
	{
		static char buffer[4];
		snprintf(buffer, 3, "%d", c);
		return buffer;
	}

<<<<<<< Updated upstream
	std::string ucs4_to_utf8_temp;
	std::string utf8_to_llvm_temp;

	std::stringstream stream;
=======
	bool mInsideUptrace;
	bool mInsideComboName;
	bool mInsideParamList;

	std::vector<TypeSpecifier*> mRepeatTypeSet;

	int addToRepeatTypeSet(TypeSpecifier* type_specifier)
	{
		auto p = std::find_if(mRepeatTypeSet.begin(), mRepeatTypeSet.end(),
				[&](TypeSpecifier* other) { return type_specifier->isEqual(*other); } );
		if(p != mRepeatTypeSet.end())
			return std::distance(mRepeatTypeSet.begin(), p);
		else
		{
			mRepeatTypeSet.push_back(type_specifier);
			return -1; // can't find type, require visitation
		}
	}

	void clearRepeatTypeSet()
	{
		mRepeatTypeSet.clear();
	}

	void addDummyTypeToRepeatTypeSet()
	{
		mRepeatTypeSet.push_back(NULL);
	}

	void writeRepeatTypeAlias(int type_index)
	{
		if(type_index > 0)
			stream << "S" << (type_index-1) << "_";
		else
			stream << "S_";
	}
>>>>>>> Stashed changes
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_ */
