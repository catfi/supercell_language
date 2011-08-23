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

#ifndef ZILLIANS_LANGUAGE_STAGE_GENERATOR_DETAIL_LLVMFOREACH_H_
#define ZILLIANS_LANGUAGE_STAGE_GENERATOR_DETAIL_LLVMFOREACH_H_

#include "utility/TemplateTricks.h"
#include "llvm/Function.h"
#include "llvm/BasicBlock.h"
#include "llvm/Module.h"
#include "llvm/Metadata.h"

static inline llvm::Function::iterator beginof (llvm::Function& m) { return m.begin(); }
static inline llvm::Function::iterator endof (llvm::Function& m)   { return m.end(); }

static inline llvm::Function::const_iterator beginof (const llvm::Function& m) { return m.begin(); }
static inline llvm::Function::const_iterator endof (const llvm::Function& m)   { return m.end(); }

static inline llvm::Function::iterator beginof (llvm::Function*& m) { return m->begin(); }
static inline llvm::Function::iterator endof (llvm::Function*& m)   { return m->end(); }

static inline llvm::Function::const_iterator beginof (const llvm::Function*& m) { return m->begin(); }
static inline llvm::Function::const_iterator endof (const llvm::Function*& m)   { return m->end(); }


static inline llvm::BasicBlock::iterator beginof (llvm::BasicBlock& m) { return m.begin(); }
static inline llvm::BasicBlock::iterator endof (llvm::BasicBlock& m)   { return m.end(); }

static inline llvm::BasicBlock::const_iterator beginof (const llvm::BasicBlock& m) { return m.begin(); }
static inline llvm::BasicBlock::const_iterator endof (const llvm::BasicBlock& m)   { return m.end(); }

static inline llvm::BasicBlock::iterator beginof (llvm::BasicBlock*& m) { return m->begin(); }
static inline llvm::BasicBlock::iterator endof (llvm::BasicBlock*& m)   { return m->end(); }

static inline llvm::BasicBlock::const_iterator beginof (const llvm::BasicBlock*& m) { return m->begin(); }
static inline llvm::BasicBlock::const_iterator endof (const llvm::BasicBlock*& m)   { return m->end(); }


static inline llvm::Module::iterator beginof (llvm::Module& m) { return m.begin(); }
static inline llvm::Module::iterator endof (llvm::Module& m)   { return m.end(); }

static inline llvm::Module::const_iterator beginof (const llvm::Module& m) { return m.begin(); }
static inline llvm::Module::const_iterator endof (const llvm::Module& m)   { return m.end(); }

static inline llvm::Module::iterator beginof (llvm::Module*& m) { return m->begin(); }
static inline llvm::Module::iterator endof (llvm::Module*& m)   { return m->end(); }

static inline llvm::Module::const_iterator beginof (const llvm::Module*& m) { return m->begin(); }
static inline llvm::Module::const_iterator endof (const llvm::Module*& m)   { return m->end(); }

#endif /* ZILLIANS_LANGUAGE_STAGE_GENERATOR_LLVMFOREACH_H_ */
