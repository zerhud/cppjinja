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
#include "context_objects/callable_params.hpp"
#include "eval/ast.hpp"

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

class raii_push_shadow_ctx final {
	context* ctx;
	const node* maker;
public:
	raii_push_shadow_ctx(raii_push_shadow_ctx&&) noexcept ;
	raii_push_shadow_ctx(const raii_push_shadow_ctx&) =delete ;
	raii_push_shadow_ctx(const node* n, context* c);
	~raii_push_shadow_ctx() ;
};

class raii_result_format final {
	result_formatter* fmt;
	int back;
public:
	raii_result_format(raii_result_format&& other) noexcept;
	raii_result_format& operator = (raii_result_format&& other) noexcept;
	raii_result_format(const raii_result_format& other) =delete ;
	raii_result_format& operator = (const raii_result_format& other) =delete;

	raii_result_format(result_formatter* f, int s, int b);
	~raii_result_format();
};

class raii_callstack_push final {
	callstack* calls;
public:
	raii_callstack_push(const raii_callstack_push&) =delete ;
	raii_callstack_push& operator = (const raii_callstack_push&) =delete ;
	raii_callstack_push(raii_callstack_push&&) noexcept ;
	raii_callstack_push& operator = (raii_callstack_push&&) noexcept ;

	raii_callstack_push(
	        callstack* c,
	        const evtnodes::callable* n,
	        context_objects::callable_params params);
	~raii_callstack_push();
};

} // namespace cppjinja::evt
