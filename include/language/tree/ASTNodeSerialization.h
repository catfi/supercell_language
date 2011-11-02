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
 * @date Sep 5, 2011 yoco - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_
#define ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_

#include "ASTNodeFactory.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace boost { namespace serialization {

// boost::tuple
#define GENERATE_ELEMENT_SERIALIZE(z,which,unused) \
    ar & boost::serialization::make_nvp("element",t.get< which >());

#define GENERATE_TUPLE_SERIALIZE(z,nargs,unused)                        \
    template< typename Archive, BOOST_PP_ENUM_PARAMS(nargs,typename T) > \
    void serialize(Archive & ar,                                        \
                   boost::tuple< BOOST_PP_ENUM_PARAMS(nargs,T) > & t,   \
                   const unsigned int version)                          \
    {                                                                   \
      BOOST_PP_REPEAT_FROM_TO(0,nargs,GENERATE_ELEMENT_SERIALIZE,~);    \
    }

    BOOST_PP_REPEAT_FROM_TO(1,6,GENERATE_TUPLE_SERIALIZE,~);

} }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODESERIALIZATION_H_ */
