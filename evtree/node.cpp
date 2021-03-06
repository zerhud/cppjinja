/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "node.hpp"
#include "parser/helpers.hpp"
#include "evtree.hpp"
#include "eval/ast.hpp"
#include "helpers/loop_by_three.hpp"
#include "exenv/callstack.hpp"

using namespace std::literals;

void cppjinja::evt::node::render_children(
        const std::pmr::vector<const node*>& children,
        exenv& ctx, render_info default_ri) const
{
	auto rnd = [&ctx,&default_ri]
	    (const node* p, const node* c, const node* n)
	{
		render_info ri = default_ri;
		if(p) ri.trim_left = p->rinfo().trim_right;
		if(n) ri.trim_right = n->rinfo().trim_left;
		ctx.rinfo(std::move(ri));
		c->render(ctx);
	};
	evtnodes::loop_by_three(children, rnd);
}

std::pmr::string cppjinja::evt::node::tps(const std::string& src)
{
	return std::pmr::string(src.begin(),src.end());
}

std::pmr::string cppjinja::evt::node::tps(const ast::expr_ops::single_var_name& src)
{
	return tps(src.name);
}
