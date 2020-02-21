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

cppjinja::evtnodes::content_block::content_block(
		render_info ri, ast::string_t n)
    : rinfo_(std::move(ri))
    , name_(std::move(n))
{
}

cppjinja::evt::node::render_info
cppjinja::evtnodes::content_block::rinfo() const
{
	return rinfo_;
}

cppjinja::ast::string_t cppjinja::evtnodes::content_block::name() const
{
	return name_;
}

bool cppjinja::evtnodes::content_block::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::content_block::render(evt::context& ctx) const
{
	ctx.current_node(this);
	ctx.push_context(this);
	auto children = ctx.tree().children(this, true);
	for(auto&& child:children) ctx.add_context(child);
	children = ctx.tree().children(this, false);
	for(auto&& child:children) child->render(ctx);
	ctx.pop_context(this);
}

