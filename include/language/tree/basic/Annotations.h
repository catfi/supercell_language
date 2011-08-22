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

#ifndef ZILLIANS_LANGUAGE_TREE_ANNOTATIONS_H_
#define ZILLIANS_LANGUAGE_TREE_ANNOTATIONS_H_

#include "language/tree/ASTNode.h"
#include "language/tree/basic/Identifier.h"

namespace zillians { namespace language { namespace tree {

struct Annotation : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Annotation, (Annotation)(ASTNode));

	Annotation(SimpleIdentifier* name) : name(name)
	{
		name->parent = this;
	}

	void appendKeyValue(SimpleIdentifier* key, ASTNode* value)
	{
		key->parent = this;
		value->parent = this;
		attribute_list.push_back(std::make_pair(key, value));
	}

	SimpleIdentifier* name;
	std::vector<std::pair<ASTNode*/*key*/, ASTNode*/*value*/>> attribute_list;
};

struct Annotations : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Annotations, (Annotations)(ASTNode));

	void appendAnnotation(Annotation* annotation)
	{
		annotation->parent = this;
		annotation_list.push_back(annotation);
	}

	std::vector<Annotation*> annotation_list;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ANNOTATIONS_H_ */
