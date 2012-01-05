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

#include "language/tree/basic/FunctionType.h"
#include "language/tree/basic/TypeSpecifier.h"

namespace zillians { namespace language { namespace tree {

void FunctionType::appendParameterType(TypeSpecifier* type)
{
	type->parent = this;
	parameter_types.push_back(type);
}

std::wstring FunctionType::toString() const
{
	std::wstringstream ss;

	ss << L"function(";
	foreach(i, parameter_types)
	{
		ss << (*i)->toString();
		if(!is_end_of_foreach(i, templated_parameters))
			ss << L", ";
	}
	ss << L")";
	if(return_type)
	{
		ss << L":";
		ss << return_type->toString();
	}

	return ss.str();
}

ASTNode* FunctionType::clone() const
{
	FunctionType* cloned = new FunctionType();
	foreach(i, parameter_types) cloned->parameter_types.push_back( (*i) ? cast<TypeSpecifier>((*i)->clone()) : NULL);
	cloned->return_type = (return_type) ? cast<TypeSpecifier>(return_type->clone()) : NULL;
	return cloned;
}

} } }
