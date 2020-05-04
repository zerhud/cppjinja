/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "block_with_name.hpp"
#include "evtree/exenv/raii.hpp"

namespace cppjinja::evtnodes {

class block_macro : public block_with_name {
	ast::block_macro block;
	const ast::block_with_name& cur_ast() const override ;
public:
	block_macro(ast::block_macro nb);
	void render(evt::exenv& env) const override ;
	east::string_t evaluate( evt::exenv& env) const override ;
};

} // namespace cppjinja::evtnodes
