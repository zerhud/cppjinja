/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "callable.hpp"

namespace cppjinja::evtnodes {

class block_call : public callable {
	ast::block_call ast;
	evt::render_info inner_ri() const ;
public:
	block_call(ast::block_call nb);
	void render(evt::exenv& env) const override ;
	evt::render_info rinfo() const override ;
	east::string_t evaluate(evt::exenv& env) const override ;
	std::vector<east::function_parameter> solved_params(evt::exenv& env) const override ;
	ast::string_t name() const override ;
};

} // namespace cppjinja::evtnodes
