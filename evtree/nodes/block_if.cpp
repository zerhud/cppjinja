/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_if.hpp"
#include "../evtree.hpp"
#include "../exenv/expr_solver.hpp"

using namespace std::literals;
using namespace cppjinja::evtnodes;

cppjinja::evt::render_info block_if::rinfo_for_children() const
{
	return evt::render_info{ block.left_close.trim, block.right_open.trim };
}

block_if::block_if(cppjinja::ast::block_if nb) : block(std::move(nb))
{
}

cppjinja::evt::render_info block_if::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

cppjinja::ast::string_t block_if::name() const
{
	return "if"s;
}

bool block_if::is_leaf() const
{
	return false;
}

void block_if::render(evt::exenv& ctx) const
{
	ctx.ctx().current_node(this);
	auto ifresult = evt::expr_solver(&ctx)(block.condition) == east::value_term{1};

	auto children = ctx.tmpl().children(this);
	assert(!children.empty() && children.size() < 3);

	ctx.rinfo(rinfo_for_children());
	if(ifresult) children[0]->render(ctx);
	if(children.size()==2 && !ifresult) children[1]->render(ctx);
}
