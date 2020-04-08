/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_named.hpp"
#include "parser/helpers.hpp"
#include "../evtree.hpp"

cppjinja::evtnodes::block_named::block_named(cppjinja::ast::block_named nb)
    : block(std::move(nb))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::block_named::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_named::name() const
{
	return block.name;
}

bool cppjinja::evtnodes::block_named::is_leaf() const
{
	return false;
}

std::optional<cppjinja::ast::value_term> cppjinja::evtnodes::block_named::param(
        const evt::callstack& ctx, const cppjinja::ast::var_name& name) const
{
	return param(block.params, name, ctx);
}

void cppjinja::evtnodes::block_named::render(evt::exenv& ctx) const
{
	evt::render_info ri{ block.left_close.trim, block.right_open.trim };
	auto children = ctx.tmpl().children(this, false);
	ctx.ctx().current_node(this);
	evt::raii_push_ctx ctx_maker(this, &ctx.ctx());
	render_children(children, ctx, ri);
}
