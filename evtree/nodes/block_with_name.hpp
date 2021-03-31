/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "callable.hpp"

namespace cppjinja::evtnodes {

class block_with_name : public callable {
	virtual const ast::block_with_name& cur_ast() const =0 ;
	[[nodiscard]] evt::raii_result_format result_format_raii(evt::exenv& env) const ;
protected:
	evt::render_info inner_ri() const ;
	[[nodiscard]] evt::raii_result_format inner_evaluate(evt::exenv& env) const ;
public:
	evt::render_info rinfo() const override ;
	std::pmr::string name() const override ;
	std::pmr::vector<east::function_parameter> solved_params(evt::exenv& env) const override ;
};

} // namespace cppjinja::evtnodes
