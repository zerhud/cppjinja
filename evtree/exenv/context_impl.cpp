/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context_impl.hpp"
#include "eval/ast_cvt.hpp"
#include "nodes/callable.hpp"
#include "ctx_object.hpp"

using namespace cppjinja::details;

void cppjinja::evt::context_impl::require_not_empty() const
{
	if(ctx.empty()) throw std::runtime_error("context is mepty");
}

cppjinja::evt::context_impl::context_impl()
{
}

cppjinja::evt::context_impl::~context_impl() noexcept
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
	ctx.emplace_back(frame{n, {}, std::stringstream{}, obj_holder()});
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

cppjinja::evt::obj_holder& cppjinja::evt::context_impl::cur_namespace()
{
	require_not_empty();
	return ctx.back().ns;
}

void cppjinja::evt::context_impl::inject_obj(
          cppjinja::ast::string_t name
        , std::shared_ptr<ctx_object> obj)
{
	require_not_empty();
	ctx.back().ns.add(name, std::move(obj));
}

void cppjinja::evt::context_impl::takeout_obj(const ast::string_t& name)
{
	require_not_empty();
	ctx.back().ns.rem(name);
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::context_impl::solve_var(const cppjinja::ast::var_name& var) const
{
	require_not_empty();
	return ctx.back().ns.solve(var);
}

std::optional<cppjinja::ast::value_term> cppjinja::evt::context_impl::solve_call(
        const cppjinja::ast::function_call& call) const
{
	require_not_empty();
	return ctx.back().ns.call(call);
}
