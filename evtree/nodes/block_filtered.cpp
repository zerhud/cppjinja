/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_filtered.hpp"

cppjinja::evtnodes::block_filtered::block_filtered(cppjinja::ast::block_filtered nb)
    : ast(std::move(nb))
{
}

void cppjinja::evtnodes::block_filtered::render(cppjinja::evt::exenv& env) const
{

}

cppjinja::evt::render_info cppjinja::evtnodes::block_filtered::rinfo() const
{
	return {ast.left_open.trim, ast.right_close.trim};
}
