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
	const exenv* env;

	east::value_term make_array(
	        const std::vector<ast::forward_ast<ast::value_term>>& fields) const ;
	std::optional<ast::value_term> search_in_params(
	        const cppjinja::ast::var_name& var) const;
public:
	typedef east::value_term ret_t;

	expr_solver(const exenv* e);

	ret_t operator()(const ast::value_term& val) const ;

	ret_t operator()(const double& obj) const ;
	ret_t operator()(const ast::string_t& obj) const ;
	ret_t operator()(const ast::tuple_v& obj) const ;
	ret_t operator()(const ast::array_v& obj) const ;
	ret_t operator()(const ast::var_name& obj) const ;
	ret_t operator()(const ast::function_call& obj) const ;
	ret_t operator()(const ast::binary_op& op) const ;
};

} // namespace cppjinja::evt
