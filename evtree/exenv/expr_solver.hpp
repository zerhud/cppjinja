/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/ast.hpp"
#include "parser/ast/common.hpp"
#include "evtree/declarations.hpp"

namespace cppjinja::evt {

class expr_solver final {
	exenv* env;

	east::value_term make_array(
	        const std::vector<ast::forward_ast<ast::value_term>>& fields) ;
	std::optional<ast::value_term> search_in_params(
	        const cppjinja::ast::var_name& var) const;
	east::value_term solve_in_data(const cppjinja::ast::function_call& obj) ;
	east::value_term solve_in_tmpl(
	          const cppjinja::ast::function_call& obj
	        , const evtnodes::callable* node) ;
	bool can_be_solved_in_tmpl(const ast::var_name& name) const ;
public:
	typedef east::value_term ret_t;

	expr_solver(exenv* e);

	ret_t operator()(const ast::value_term& val) ;

	ret_t operator()(const double& obj) const ;
	ret_t operator()(const ast::string_t& obj) const ;
	ret_t operator()(const ast::tuple_v& obj) ;
	ret_t operator()(const ast::array_v& obj) ;
	ret_t operator()(const ast::var_name& obj) ;
	ret_t operator()(const ast::function_call& obj) ;
	ret_t operator()(const ast::binary_op& op) ;
};

} // namespace cppjinja::evt
