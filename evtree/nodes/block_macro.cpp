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
#include "evtree/exenv/expr_solver.hpp"

const cppjinja::ast::block_with_name& cppjinja::evtnodes::block_macro::cur_ast() const
{
	return block;
}

cppjinja::evtnodes::block_macro::block_macro(cppjinja::ast::block_macro nb)
    : block(std::move(nb))
{
}

void cppjinja::evtnodes::block_macro::render(evt::exenv& env) const
{
	env.current_node(this);
}

cppjinja::east::string_t
cppjinja::evtnodes::block_macro::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	auto fmt = inner_evaluate(env);
	return east::string_t();
}
