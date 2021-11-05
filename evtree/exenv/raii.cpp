/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "raii.hpp"
#include "context.hpp"
#include "callstack.hpp"
#include "result_formatter.hpp"

using cppjinja::evt::raii_result_format;
using cppjinja::evt::raii_callstack_push;

cppjinja::evt::raii_push_ctx::raii_push_ctx(
        cppjinja::evt::raii_push_ctx&& other) noexcept
    : ctx(other.ctx)
    , maker(other.maker)
{
	other.ctx = nullptr;
	other.maker = nullptr;
}

cppjinja::evt::raii_push_ctx::raii_push_ctx(
        const cppjinja::evt::node* n, cppjinja::evt::context* c)
    : ctx(c)
    , maker(n)
{
	if(maker) ctx->push(maker);
	else ctx = nullptr;
}

cppjinja::evt::raii_push_ctx::~raii_push_ctx()
{
	if(ctx) ctx->pop(maker);
}

raii_result_format::raii_result_format(raii_result_format&& other) noexcept
    : fmt(other.fmt), back(other.back)
{
	other.fmt = nullptr;
}

raii_result_format& raii_result_format::operator = (raii_result_format&& other) noexcept
{
	fmt = other.fmt;
	back = other.back;
	other.fmt = nullptr;
	return *this;
}

raii_result_format::raii_result_format(
        cppjinja::evt::result_formatter* f, int s, int b)
    : fmt(f) , back(b)
{
	fmt->shift_tab(s);
}

raii_result_format::~raii_result_format()
{
	if(fmt) fmt->shift_tab(back);
}

raii_callstack_push::raii_callstack_push(raii_callstack_push&& other) noexcept
    : calls(other.calls)
{
	other.calls = nullptr;
}

raii_callstack_push& raii_callstack_push::operator = (raii_callstack_push&& other) noexcept
{
	calls = other.calls;
	other.calls = nullptr;
	return *this;
}

raii_callstack_push::raii_callstack_push(
          cppjinja::evt::callstack* c
        , const cppjinja::evtnodes::callable* n
        , context_objects::callable_params params)
    : calls(c)
{
	if(n) calls->push(n, std::move(params));
	else calls = nullptr;
}

raii_callstack_push::~raii_callstack_push()
{
	if(calls) calls->pop();
}

cppjinja::evt::raii_push_shadow_ctx::raii_push_shadow_ctx(
        cppjinja::evt::raii_push_shadow_ctx&& other) noexcept
    : ctx(other.ctx)
    , maker(other.maker)
{
	other.ctx = nullptr;
	other.maker = nullptr;
}

cppjinja::evt::raii_push_shadow_ctx::raii_push_shadow_ctx(
        const cppjinja::evt::node* n, cppjinja::evt::context* c)
    : ctx(c)
    , maker(n)
{
	if(maker) ctx->push_shadow(maker);
	else ctx = nullptr;
}

cppjinja::evt::raii_push_shadow_ctx::~raii_push_shadow_ctx()
{
	if(ctx) ctx->pop(maker);
}
