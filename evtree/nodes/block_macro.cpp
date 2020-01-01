/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <istream>
#include "block_macro.hpp"

cppjinja::evtnodes::block_macro::block_macro(cppjinja::ast::block_macro nb)
    : block(std::move(nb))
{
}

cppjinja::node::render_info cppjinja::evtnodes::block_macro::rinfo() const
{
	return render_info{ false, false };
}

cppjinja::ast::string_t cppjinja::evtnodes::block_macro::name() const
{
	return block.name;
}

bool cppjinja::evtnodes::block_macro::is_leaf() const
{
	return true;
}

void cppjinja::evtnodes::block_macro::render(
          std::ostream& out
        , const cppjinja::evtree& tree
        , evt::context& ctx
        ) const
{
	out << "block_macro" ;
}
