/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"

cppjinja::evt::context::context(const data_provider* p)
    : prov(p)
{
}

const cppjinja::data_provider& cppjinja::evt::context::data() const
{
	return *prov;
}

void cppjinja::evt::context::pop_context()
{
	assert( !ctx.empty() );
	ctx.pop_back();
}

void cppjinja::evt::context::push_context()
{
	ctx.push_back({});
}

void cppjinja::evt::context::add_context(const node* n)
{
	assert( !ctx.empty() );
	ctx.back().emplace_back(n);
}

std::vector<const cppjinja::node*> cppjinja::evt::context::ctx_all() const
{
	assert( !ctx.empty() );
	return ctx.back();
}

const cppjinja::node* cppjinja::evt::context::ctx_last() const
{
	assert( !ctx.empty() );
	if(ctx.back().empty()) return nullptr;
	return ctx.back().back();
}

void cppjinja::evt::context::pop_callstack(const cppjinja::node* n)
{
	assert( !callstack.empty() );
	assert( callstack.back() == n );
	callstack.pop_back();
	pop_context();
}

void cppjinja::evt::context::push_callstack(const cppjinja::node* n)
{
	callstack.push_back(n);
	push_context();
}
