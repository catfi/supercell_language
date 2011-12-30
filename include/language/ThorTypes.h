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

#ifndef ZILLIANS_LANGUAGE_THORTYPES_H_
#define ZILLIANS_LANGUAGE_THORTYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

namespace zillians { namespace language {

// define common types
typedef void ts_void;
typedef bool ts_boolean;

typedef int8_t ts_int8;
typedef int16_t ts_int16;
typedef int32_t ts_int32;
typedef long long ts_int64;

typedef uint8_t ts_uint8;
typedef uint16_t ts_uint16;
typedef uint32_t ts_uint32;
typedef unsigned long long ts_uint64;

typedef float ts_float32;
typedef double ts_float64;

typedef uint32_t ts_object;
typedef unsigned long long ts_callback; // first 32bit for game id, second 32bit for function id

typedef ts_uint8* ts_pointer;

} }

#endif /* ZILLIANS_LANGUAGE_THORTYPES_H_ */
