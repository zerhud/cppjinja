/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_if.hpp"
#include "../evtree.hpp"

using namespace std::literals;
using namespace cppjinja::evtnodes;

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

void block_if::render(evt::context& ctx) const
{
	ctx.current_node(this);
	auto ifresult = calculate(ctx, block.condition);

	auto children = ctx.tree().children(this);
	assert(!children.empty() && children.size() < 3);

	evt::render_info ri{ block.left_close.trim, block.right_open.trim };
	ctx.cur_render_info(ri);
	if(ifresult) children[0]->render(ctx);
	if(children.size()==2 && !ifresult) children[1]->render(ctx);
}
