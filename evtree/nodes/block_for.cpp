/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_for.hpp"

cppjinja::evtnodes::block_for::block_for(cppjinja::ast::block_for ast_bl)
    : abl(ast_bl)
{
}

cppjinja::evt::render_info cppjinja::evtnodes::block_for::rinfo() const
{
	return {};
}

void cppjinja::evtnodes::block_for::render(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	evt::raii_push_ctx ctx(this, &env.ctx());
}
