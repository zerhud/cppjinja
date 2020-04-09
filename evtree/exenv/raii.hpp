/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "evtree/declarations.hpp"

namespace cppjinja::evt {

class raii_push_ctx final {
	context* ctx;
	const node* maker;
public:
	raii_push_ctx(const node* n, context* c);
	~raii_push_ctx() ;
};

class raii_inject final {
	context* ctx;
	const evtnodes::callable* maker;
public:
	raii_inject(const evtnodes::callable* n, context* c);
	~raii_inject() ;
};

class raii_push_callstack {
	callstack* ctx;
	const node* maker;
public:
	raii_push_callstack(const node* n, const evtnodes::callable* c, callstack* m);
	~raii_push_callstack() ;
};

} // namespace cppjinja::evt
