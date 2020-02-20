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

cppjinja::evt::node::render_info block_if::rinfo() const
{
	return render_info{
		{block.left_open.trim, block.right_close.trim},
		{block.left_close.trim, block.right_open.trim}
	};
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
	if(!calculate(ctx, block.condition)) return;

	auto children = ctx.tree().children(this);
	for(auto&& child:children) child->render(ctx);
}
