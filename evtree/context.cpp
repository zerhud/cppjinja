/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"
#include "evtree.hpp"

using namespace std::literals;

cppjinja::evt::context::context(
          const data_provider* p
        , const evtree* t
        , std::ostream& out
        )
    : prov(p), tree_(t), outstream(out)
{
}

std::ostream& cppjinja::evt::context::out()
{
	return outstream;
}

const cppjinja::data_provider& cppjinja::evt::context::data() const
{
	return *prov;
}

const cppjinja::evtree& cppjinja::evt::context::tree() const
{
	assert(tree_);
	return *tree_;
}

void cppjinja::evt::context::render_variable(
        const cppjinja::ast::var_name& var)
{
	auto inner_node = tree_->search(var, cur_node);
	if(inner_node) inner_node->render(*this);
	else out() << prov->render(var);
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

void cppjinja::evt::context::add_context(const evt::node* n)
{
	assert( !ctx.empty() );
	ctx.back().emplace_back(n);
}

std::vector<const cppjinja::evt::node*> cppjinja::evt::context::ctx_all() const
{
	assert( !ctx.empty() );
	return ctx.back();
}

const cppjinja::evt::node* cppjinja::evt::context::ctx_last() const
{
	assert( !ctx.empty() );
	if(ctx.back().empty()) return nullptr;
	return ctx.back().back();
}

void cppjinja::evt::context::current_node(const cppjinja::evt::node* n)
{
	/*
	if(ctx.empty())
	{
		cur_node = n;
		return;
	}

	auto& cur_ctx = ctx.back();
	auto cur_pos = std::find(std::begin(cur_ctx), std::end(cur_ctx), cur_node);
	assert(cur_pos==std::end(cur_ctx) || cur_node!=nullptr);
	if(n!=nullptr)
	{
		auto n_pos = std::find(std::begin(cur_ctx), std::end(cur_ctx), n);
		if( n_pos==std::end(cur_ctx) )
			throw std::runtime_error("cannot find new current node");
		if( ++cur_pos != n_pos )
			throw std::runtime_error("callstack error");
	}
	*/

	cur_node = n;
}

const cppjinja::evt::node* cppjinja::evt::context::current_node() const
{
	return cur_node;
}

const cppjinja::evt::node* cppjinja::evt::context::caller() const
{
	assert( !callstack.empty() );
	return callstack.back().caller;
}

void cppjinja::evt::context::pop_callstack(const cppjinja::evt::node* n)
{
	assert( !callstack.empty() );
	assert( callstack.back().caller == n );
	callstack.pop_back();
	pop_context();
}

void cppjinja::evt::context::push_callstack(const cppjinja::evt::node* n)
{
	callstack.push_back(callstack_frame{n, {}});
	push_context();
}

std::vector<cppjinja::ast::function_call_parameter>
cppjinja::evt::context::call_params() const
{
	assert( !callstack.empty() );
	return callstack.back().params;
}

void cppjinja::evt::context::call_params(
        std::vector<cppjinja::ast::function_call_parameter> params)
{
	assert( !callstack.empty() );
	callstack.back().params = std::move(params);
}
