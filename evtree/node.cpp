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
#include "helpers/binary_op.hpp"
#include "helpers/loop_by_three.hpp"
#include "exenv/expr_solver.hpp"
#include "exenv/callstack.hpp"

using namespace std::literals;

void cppjinja::evt::node::render_children(
        const std::vector<const node*>& children,
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
