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

	east::function_parameter reduce(const ast::function_call_parameter& obj);

	east::string_t reduce_to_string(const east::value_term& obj);
public:
	typedef east::value_term ret_t;

	expr_solver(exenv* e);

	east::var_name reduce(const ast::var_name& obj) ;
	east::var_name reduce(const ast::array_calls& obj);
	east::function_call reduce(const ast::function_call& obj);
	east::var_name reduce(const boost::spirit::x3::variant<ast::var_name, ast::array_calls>& obj);
	std::vector<east::function_parameter> reduce(const std::vector<ast::function_call_parameter>& obj);

	ret_t operator()(const ast::value_term& val) ;

	ret_t operator()(const double& obj) const ;
	ret_t operator()(const ast::string_t& obj) const ;
	ret_t operator()(const ast::tuple_v& obj) ;
	ret_t operator()(const ast::array_v& obj) ;
	ret_t operator()(const ast::var_name& obj) ;
	ret_t operator()(const ast::function_call& obj) ;
	ret_t operator()(const ast::binary_op& op) ;
	ret_t operator()(const ast::array_calls& op) ;
};

} // namespace cppjinja::evt
