/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "op_out.hpp"

using namespace std::literals;

cppjinja::evtnodes::op_out::op_out(cppjinja::ast::op_out o)
    : op(std::move(o))
{
}

cppjinja::node::render_info cppjinja::evtnodes::op_out::rinfo() const
{
	return render_info{ op.open.trim, op.close.trim };
}

cppjinja::ast::string_t cppjinja::evtnodes::op_out::name() const
{
	return "op_out"s;
}

bool cppjinja::evtnodes::op_out::is_leaf() const
{
	return true;
}

void cppjinja::evtnodes::op_out::render(
          std::ostream& out
        , const cppjinja::evtree& tree
        , cppjinja::evt::context& ctx
        ) const
{
	(void) tree;
	(void) ctx;
	out << "op_out"sv ;
}
