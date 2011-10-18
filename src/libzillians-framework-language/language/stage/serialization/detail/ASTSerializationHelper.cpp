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

#include "language/stage/serialization/detail/ASTSerializationHelper.h"
#include "language/stage/serialization/visitor/ASTDeserializationStageVisitor.h"
#include "language/stage/serialization/visitor/ASTSerializationStageVisitor.h"

namespace zillians { namespace language { namespace stage {

bool ASTSerializationHelper::serialize(const std::string& filename, tree::ASTNode* node)
{
    std::ofstream ofs(filename);
    if(!ofs.good()) return false;

    // serialize through boost archive
    boost::archive::text_oarchive oa(ofs);
    tree::ASTNode* to_serialize = node;
    oa << to_serialize;

	// de-serialize all objects attached to ContextHub
	// see ASTSerializationStageVisitor::FullDeserializer, which defines the context object types needed to be serialized
    visitor::ASTSerializationStageVisitor<boost::archive::text_oarchive> serialzer(oa);
    serialzer.visit(*to_serialize);
}

tree::ASTNode* ASTSerializationHelper::deserialize(const std::string& filename)
{
	std::ifstream ifs(filename);
	if(!ifs.good()) return NULL;

	// de-serialize through boost archive
	boost::archive::text_iarchive ia(ifs);
	tree::ASTNode* from_serialize = NULL;
	ia >> from_serialize;

	// de-serialize all objects attached to ContextHub
	// see ASTDeserializationStageVisitor::FullDeserializer, which defines the context object types needed to be de-serialized
	visitor::ASTDeserializationStageVisitor<boost::archive::text_iarchive> deserialzer(ia);
	deserialzer.visit(*from_serialize);

	return from_serialize;
}

} } }
