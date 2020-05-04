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
	ctx->push(maker);
}

cppjinja::evt::raii_push_ctx::~raii_push_ctx()
{
	if(ctx) ctx->pop(maker);
}

cppjinja::evt::raii_result_format::raii_result_format(
        cppjinja::evt::result_formatter* f, int s, int b)
    : fmt(f) , back(b)
{
	fmt->shift_tab(s);
}

cppjinja::evt::raii_result_format::~raii_result_format()
{
	fmt->shift_tab(back);
}

cppjinja::evt::raii_callstack_push::raii_callstack_push(
          cppjinja::evt::callstack* c
        , const cppjinja::evtnodes::callable* n
        , context_objects::callable_params params)
    : calls(c)
{
	calls->push(n, std::move(params));
}

cppjinja::evt::raii_callstack_push::~raii_callstack_push()
{
	calls->pop();
}
