/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "block_with_name.hpp"

namespace cppjinja::evtnodes {

class block_call : public block_with_name {
	ast::block_call ast;
	const ast::block_with_name& cur_ast() const override ;
public:
	block_call(ast::block_call nb);
	void render(evt::exenv& env) const override ;
	absd::data evaluate(evt::exenv& env) const override ;
	std::pmr::string name() const override ;
};

} // namespace cppjinja::evtnodes
