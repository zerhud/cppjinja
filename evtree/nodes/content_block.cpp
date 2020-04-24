/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "content_block.hpp"
#include "parser/helpers.hpp"
#include "../evtree.hpp"

cppjinja::evtnodes::content_block::content_block(evt::render_info ri)
    : rinfo_(std::move(ri))
{
}

cppjinja::evt::render_info
cppjinja::evtnodes::content_block::rinfo() const
{
	return rinfo_;
}

bool cppjinja::evtnodes::content_block::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::content_block::render(evt::exenv& ctx) const
{
	auto def_ri = ctx.rinfo();
	if(!def_ri) def_ri = rinfo();
	auto children = ctx.children(this);
	ctx.current_node(this);
	render_children(children, ctx, *def_ri);
}
