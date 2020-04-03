/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <istream>
#include "block_macro.hpp"
#include "../evtree.hpp"

cppjinja::evtnodes::block_macro::block_macro(cppjinja::ast::block_macro nb)
    : block(std::move(nb))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::block_macro::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_macro::name() const
{
	return block.name;
}

bool cppjinja::evtnodes::block_macro::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::block_macro::render(evt::context& ctx) const
{
	evt::render_info ri{ block.left_close.trim, block.right_open.trim };
	auto children = ctx.tree().children(this, false);
	render_children_wc(children, ctx, ri);
}

std::optional<cppjinja::ast::value_term>
cppjinja::evtnodes::block_macro::param(
          cppjinja::evt::context& ctx
        , const cppjinja::ast::var_name& name
        ) const
{
	(void)ctx;
	(void)name;
	return std::nullopt;
}
