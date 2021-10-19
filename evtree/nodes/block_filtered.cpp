/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_filtered.hpp"
#include "evtree/exenv/expr_eval.hpp"

cppjinja::evt::render_info cppjinja::evtnodes::block_filtered::inner_ri() const
{
	return evt::render_info{ ast.left_close.trim, ast.right_open.trim };
}

cppjinja::evtnodes::block_filtered::block_filtered(cppjinja::ast::block_filtered nb)
    : ast(std::move(nb))
{
}

void cppjinja::evtnodes::block_filtered::render(cppjinja::evt::exenv& env) const
{
	auto children = env.children(this);
	env.current_node(this);
	std::string base;
	{
		evt::raii_push_ctx maker(this, &env.ctx());
		render_children(children, env, inner_ri());
		auto _base = (std::pmr::string)env.result();
		base.append(_base.begin(),_base.end());
	}
	ast::expr_ops::filter flt;
	flt.base = ast::expr_ops::expr{ast::expr_ops::term{base}};
	flt.filters.emplace_back(ast.flt);
	env.out() << evt::expr_eval(&env)(ast::expr_ops::expr{flt});
}

cppjinja::evt::render_info cppjinja::evtnodes::block_filtered::rinfo() const
{
	return {ast.left_open.trim, ast.right_close.trim};
}
