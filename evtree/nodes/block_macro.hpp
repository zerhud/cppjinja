/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "callable.hpp"
#include "evtree/exenv/raii.hpp"

namespace cppjinja::evtnodes {

class block_macro : public callable {
	ast::block_macro block;
	evt::render_info inner_ri() const ;
	[[nodiscard]] evt::raii_result_format result_format_raii(evt::exenv& env) const ;
public:
	block_macro(ast::block_macro nb);
	evt::render_info rinfo() const override ;
	ast::string_t name() const override ;
	void render(evt::exenv& env) const override ;

	east::string_t evaluate( evt::exenv& env) const override ;
	std::vector<ast::macro_parameter> params() const override ;
	std::vector<east::function_parameter> solved_params(evt::exenv& env) const override ;
};

} // namespace cppjinja::evtnodes
