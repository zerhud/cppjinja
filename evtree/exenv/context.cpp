/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"
#include "eval/ast_cvt.hpp"
#include "nodes/op_set.hpp"
#include "nodes/callable.hpp"

using namespace cppjinja::details;

void cppjinja::evt::context_new::require_not_empty() const
{
	if(ctx.empty()) throw std::runtime_error("context is mepty");
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::context_new::search_in_setts(
        const cppjinja::ast::var_name& var) const
{
	assert(!ctx.empty());
	for(auto& ctx_node:ctx.back().node_stack)
		if(auto set = dynamic_cast<const evtnodes::op_set*>(ctx_node);
		        ctx_node->name() == var[0] && set)
			return set->value(var);
	return std::nullopt;
}

cppjinja::evt::context_new::context_new()
{
}

void cppjinja::evt::context_new::current_node(const cppjinja::evt::node* n)
{
	require_not_empty();
	ctx.back().node_stack.emplace_back(n);
}

const cppjinja::evt::node*
cppjinja::evt::context_new::current_node(std::size_t ind) const
{
	require_not_empty();
	std::size_t i = ctx.back().node_stack.size() - ind - 1;
	return ctx.back().node_stack.at(i);
}

void cppjinja::evt::context_new::pop(const cppjinja::evt::node* n)
{
	require_not_empty();
	if(ctx.back().maker != n)
		throw std::runtime_error("cannot pop that wans't pushed");
	ctx.pop_back();
}

void cppjinja::evt::context_new::push(const cppjinja::evt::node* n)
{
	ctx.emplace_back(frame{n, {}, {}});
}

const cppjinja::evt::node* cppjinja::evt::context_new::maker() const
{
	require_not_empty();
	return ctx.back().maker;
}

void cppjinja::evt::context_new::inject(const cppjinja::evtnodes::callable* n)
{
	require_not_empty();
	if(n==nullptr) throw std::runtime_error("cannot inject nullptr");
	ctx.back().injections.emplace_back(n);
}

void cppjinja::evt::context_new::takeout(const cppjinja::evtnodes::callable* n)
{
	require_not_empty();
	auto& ins = ctx.back().injections;
	auto pos = std::find(ins.begin(), ins.end(), n);
	if(pos!=ins.end()) ins.erase(pos);
	else throw std::runtime_error("cannot takeout that wasn't inject");
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::context_new::injections() const
{
	require_not_empty();
	return ctx.back().injections;
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::context_new::solve_var(const cppjinja::ast::var_name& var) const
{
	require_not_empty();
	return search_in_setts(var);
}

cppjinja::evt::raii_push_ctx::raii_push_ctx(
        const cppjinja::evt::node* n, cppjinja::evt::context_new* c)
    : ctx(c)
    , maker(n)
{
	ctx->push(maker);
}

cppjinja::evt::raii_push_ctx::~raii_push_ctx()
{
	ctx->pop(maker);
}

cppjinja::evt::raii_inject::raii_inject(
        const evtnodes::callable* n, cppjinja::evt::context_new* c)
    : ctx(c)
    , maker(n)
{
	ctx->inject(maker);
}

cppjinja::evt::raii_inject::~raii_inject()
{
	ctx->takeout(maker);
}
