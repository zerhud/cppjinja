/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"
#include "evtree.hpp"
#include "eval/ast_cvt.hpp"

using namespace std::literals;

bool cppjinja::evt::context::is_filtering() const
{
	return filters_out.has_value();
}

std::string cppjinja::evt::context::reset_filtering_content()
{
	auto cnt = filters_out->str();
	filters_out->str(std::string());
	return cnt;
}

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
	return is_filtering() ? *filters_out : outstream;
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

void cppjinja::evt::context::render_filter(
        const cppjinja::ast::var_name& var)
{
	assert(is_filtering());

	ast::function_call icall{var, call_params()};
	auto call = details::east_cvt::cvt(icall);
	auto base = reset_filtering_content();
	out() << prov->render(std::move(call), base);
}

void cppjinja::evt::context::render_variable(
        const cppjinja::ast::var_name& var)
{
	auto* cb_cur = dynamic_cast<const evtnodes::callable*>(ctx_maker());
	if(cb_cur && cb_cur->render_param(*this, var)) return;

	auto* inner_node = tree_->search(var, cur_node);
	if(inner_node) inner_node->render_param(*this, var);
	else out() << prov->render(var);
}

void cppjinja::evt::context::render_function(
        const cppjinja::ast::function_call& var)
{
	auto inner_node = tree_->search(var.ref, current_node());
	if(inner_node) inner_node->render(*this);
	else out() << prov->render(details::east_cvt::cvt(var));
}

void cppjinja::evt::context::pop_context(const node* m)
{
	assert( !ctx.empty() );
	assert(ctx.back().maker == m);
	ctx.pop_back();
}

void cppjinja::evt::context::push_context(const node* m)
{
	ctx.push_back({m, {}});
}

void cppjinja::evt::context::add_context(const evt::node* n)
{
	assert( !ctx.empty() );
	ctx.back().ctx.emplace_back(n);
}

std::vector<const cppjinja::evt::node*> cppjinja::evt::context::ctx_all() const
{
	assert( !ctx.empty() );
	return ctx.back().ctx;
}

const cppjinja::evt::node* cppjinja::evt::context::ctx_last() const
{
	assert( !ctx.empty() );
	if(ctx.back().ctx.empty()) return nullptr;
	return ctx.back().ctx.back();
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

const cppjinja::evt::node* cppjinja::evt::context::ctx_maker() const
{
	assert( !ctx.empty() );
	return ctx.back().maker;
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

	if(is_filtering())
	{
		outstream << filters_out->str();
		filters_out.reset();
	}

	callstack.pop_back();
}

void cppjinja::evt::context::push_callstack(
          const cppjinja::evt::node* n
        , bool is_filtering
        )
{
	if(is_filtering) filters_out.emplace(std::string());
	callstack.push_back(callstack_frame{n, {}});
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
