/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_else.hpp"
#include "parser/helpers.hpp"
#include "../evtree.hpp"

cppjinja::evtnodes::block_else::block_else(cppjinja::ast::else_thread nb)
    : block(std::move(nb))
{
}

cppjinja::evt::node::render_info cppjinja::evtnodes::block_else::rinfo() const
{
	return render_info{
		{block.left_open.trim, false},
		//{block.left_open.trim, block.right_close.trim},
		//{block.left_close.trim, block.right_open.trim}
		{block.left_close.trim, false}
	};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_else::name() const
{
	return "else_thread";
}

bool cppjinja::evtnodes::block_else::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::block_else::render(evt::context& ctx) const
{
}

