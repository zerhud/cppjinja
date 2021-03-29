/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <istream>
#include <absd/simple_data_holder.hpp>

#include "block_macro.hpp"
#include "../evtree.hpp"

using cppjinja::evtnodes::block_macro;

const cppjinja::ast::block_with_name& block_macro::cur_ast() const
{
	return block;
}

block_macro::block_macro(cppjinja::ast::block_macro nb)
    : block(std::move(nb))
{
}

void block_macro::render(evt::exenv& env) const
{
	env.current_node(this);
}

absd::data block_macro::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	auto fmt = inner_evaluate(env);
	auto es = std::allocate_shared<absd::simple_data_holder>(
	            std::pmr::polymorphic_allocator{env.storage().get()},
	            env.storage(), "");
	return absd::data{es};
}
