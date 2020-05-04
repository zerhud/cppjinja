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

void block_if::render_if(cppjinja::evt::exenv& ctx) const
{
	auto formatter = raii_if_formatter(ctx);
	ctx.children(this).at(0)->render(ctx);
}

void block_if::render_else(cppjinja::evt::exenv& ctx) const
{
	auto children = ctx.children(this);
	if(children.size() == 2) {
		auto formatter = raii_else_formatter(ctx);
		children[1]->render(ctx);
	}
}

cppjinja::evt::raii_result_format block_if::raii_if_formatter(cppjinja::evt::exenv& ctx) const
{
	bool first_trim = block.left_close.trim;
	auto first_bsign = block.left_close.bsign.value_or(0);
	if(block.else_block.has_value()) {
		ctx.rinfo(evt::render_info{first_trim, block.else_block->left_open.trim});
		return evt::raii_result_format( &ctx.render_format(), first_bsign,
		            block.else_block->left_open.bsign.value_or(0));
	}
	ctx.rinfo(evt::render_info{first_trim, block.right_open.trim});
	return evt::raii_result_format( &ctx.render_format(), first_bsign,
	                                block.right_open.bsign.value_or(0));
}

cppjinja::evt::raii_result_format block_if::raii_else_formatter(cppjinja::evt::exenv& ctx) const
{
	assert(block.else_block);
	ctx.rinfo(evt::render_info{block.else_block->left_close.trim, block.right_open.trim});
	return evt::raii_result_format(
	            &ctx.render_format(),
	            block.else_block->left_close.bsign.value_or(0),
	            block.right_open.bsign.value_or(0)
	            );
}

block_if::block_if(cppjinja::ast::block_if nb) : block(std::move(nb))
{
}

cppjinja::evt::render_info block_if::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

void block_if::render(evt::exenv& ctx) const
{
	ctx.current_node(this);
	auto ifresult = evt::expr_solver(&ctx)(block.condition) == east::value_term{1};
	if(ifresult) render_if(ctx);
	else render_else(ctx);
}
