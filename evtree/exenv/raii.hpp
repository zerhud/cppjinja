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
	raii_push_ctx(raii_push_ctx&&) noexcept ;
	raii_push_ctx(const raii_push_ctx&) =delete ;
	raii_push_ctx(const node* n, context* c);
	~raii_push_ctx() ;
};

class raii_inject_obj final {
	context* ctx;
	std::string name;
public:
	raii_inject_obj(raii_inject_obj&&) noexcept ;
	raii_inject_obj(const raii_inject_obj&) =delete ;
	raii_inject_obj(std::string n, std::unique_ptr<ctx_object> obj, context* c);
	~raii_inject_obj() ;
};

class raii_push_callstack {
	callstack* ctx;
	const node* maker;
public:
	raii_push_callstack(raii_push_callstack&&) noexcept ;
	raii_push_callstack(const raii_push_callstack&) =delete ;
	raii_push_callstack(const node* n, const evtnodes::callable* c, callstack* m);
	raii_push_callstack(callstack* m, context* ctx, const evtnodes::callable* c);
	~raii_push_callstack() ;
};

} // namespace cppjinja::evt
