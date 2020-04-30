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

	east::value_term solve_queue(const ast::var_name& obj);
	east::value_term solve_queue(const ast::function_call& obj);
	template<typename Obj, typename Cont, typename... Conts>
	east::value_term solve_queue(const Obj& obj, const Cont& cont, const Conts&... conts);
	template<typename Obj>
	std::optional<east::value_term> solve_in_params(const Obj& obj);
	template<typename Obj> east::value_term solve_calls(const Obj& obj);

	east::value_term make_array(
	        const std::vector<ast::forward_ast<ast::value_term>>& fields) ;
	east::value_term solve_in_data(const cppjinja::ast::function_call& obj) ;
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
	ret_t operator()(const ast::array_call& op) ;
};

} // namespace cppjinja::evt
