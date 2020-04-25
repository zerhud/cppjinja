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
#include "ctx_object.hpp"

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

cppjinja::evt::raii_push_callstack::raii_push_callstack(
        cppjinja::evt::raii_push_callstack&& other) noexcept
    : ctx(other.ctx)
{
	other.ctx = nullptr;
}

cppjinja::evt::raii_push_callstack::raii_push_callstack(
          const evtnodes::callable* c
        , cppjinja::evt::callstack* m
        )
    : ctx(m)
{
	ctx->push(c);
}

cppjinja::evt::raii_push_callstack::~raii_push_callstack()
{
	if(ctx) ctx->pop();
}

cppjinja::evt::raii_inject_obj::raii_inject_obj(
        cppjinja::evt::raii_inject_obj&& other) noexcept
    : ctx(other.ctx)
    , name(std::move(other.name))
{
	other.ctx = nullptr;
}

cppjinja::evt::raii_inject_obj::raii_inject_obj(
          std::string n
        , std::unique_ptr<cppjinja::evt::ctx_object> obj
        , cppjinja::evt::context* c)
   : ctx(c)
   , name(std::move(n))
{
	ctx->inject_obj(name, std::move(obj));
}

cppjinja::evt::raii_inject_obj::~raii_inject_obj()
{
	if(ctx) ctx->takeout_obj(name);
}
