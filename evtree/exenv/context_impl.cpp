/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context_impl.hpp"
#include "eval/ast_cvt.hpp"
#include "nodes/op_set.hpp"
#include "nodes/callable.hpp"

using namespace cppjinja::details;

void cppjinja::evt::context_impl::require_not_empty() const
{
	if(ctx.empty()) throw std::runtime_error("context is mepty");
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::context_impl::search_in_setts(
        const cppjinja::ast::var_name& var) const
{
	assert(!ctx.empty());
	for(auto& ctx_node:ctx.back().node_stack)
		if(auto set = dynamic_cast<const evtnodes::op_set*>(ctx_node);
		        ctx_node->name() == var[0] && set)
			return set->value(var);
	return std::nullopt;
}

cppjinja::evt::context_impl::context_impl()
{
}

void cppjinja::evt::context_impl::current_node(const cppjinja::evt::node* n)
{
	require_not_empty();
	ctx.back().node_stack.emplace_back(n);
}

const cppjinja::evt::node*
cppjinja::evt::context_impl::nth_node_on_stack(std::size_t ind) const
{
	require_not_empty();
	std::size_t i = ctx.back().node_stack.size() - ind - 1;
	return ctx.back().node_stack.at(i);
}

void cppjinja::evt::context_impl::pop(const cppjinja::evt::node* n)
{
	require_not_empty();
	if(ctx.back().maker != n)
		throw std::runtime_error("cannot pop that wans't pushed");
	ctx.pop_back();
}

void cppjinja::evt::context_impl::push(const cppjinja::evt::node* n)
{
	ctx.emplace_back(frame{n, {}, {}, std::stringstream{}});
}

const cppjinja::evt::node* cppjinja::evt::context_impl::maker() const
{
	require_not_empty();
	return ctx.back().maker;
}

std::ostream& cppjinja::evt::context_impl::out()
{
	require_not_empty();
	return ctx.back().out;
}

std::string cppjinja::evt::context_impl::result() const
{
	require_not_empty();
	return ctx.back().out.str();
}

void cppjinja::evt::context_impl::inject(const cppjinja::evtnodes::callable* n)
{
	require_not_empty();
	if(n==nullptr) throw std::runtime_error("cannot inject nullptr");
	ctx.back().injections.emplace_back(n);
}

void cppjinja::evt::context_impl::takeout(const cppjinja::evtnodes::callable* n)
{
	require_not_empty();
	auto& ins = ctx.back().injections;
	auto pos = std::find(ins.begin(), ins.end(), n);
	if(pos!=ins.end()) ins.erase(pos);
	else throw std::runtime_error("cannot takeout that wasn't inject");
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::context_impl::injections() const
{
	require_not_empty();
	return ctx.back().injections;
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::context_impl::solve_var(const cppjinja::ast::var_name& var) const
{
	require_not_empty();
	return search_in_setts(var);
}
