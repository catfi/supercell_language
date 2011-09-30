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

void FunctionType::appendTemplateParameter(Identifier* parameter)
{
	parameter->parent = this;
	templated_parameters.push_back(parameter);
}

void FunctionType::appendParameterType(TypeSpecifier* type)
{
	type->parent = this;
	argument_types.push_back(type);
}

std::wstring FunctionType::toString() const
{
	static std::wstring t;
	std::wstringstream ss;
	ss << L"function";
	foreach(i, templated_parameters)
	{
		if(is_begin_of_foreach(i, templated_parameters))
			ss << L"<";

		ss << (*i)->toString();

		if(!is_end_of_foreach(i, templated_parameters))
			ss << L", ";
		else
			ss << L">";
	}

	ss << L"(";
	foreach(i, argument_types)
	{
		ss << (*i)->toString();
		if(!is_end_of_foreach(i, templated_parameters))
			ss << L", ";
	}
	ss << L")";

	t = ss.str();

	return t;
}

} } }
