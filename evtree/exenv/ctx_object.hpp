/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evtree/declarations.hpp"
#include "parser/ast/common.hpp"

namespace cppjinja::evt {

class ctx_object {
public:
	virtual ~ctx_object() noexcept =default ;
	virtual ast::value_term call(ast::function_call fnc) =0 ;
	virtual ast::value_term solve(ast::var_name var) =0 ;
};

class delay_solver : public ctx_object {
	const ast::value_term* val;
public:
	delay_solver(const ast::value_term* v);
	ast::value_term call(ast::function_call fnc) override ;
	ast::value_term solve(ast::var_name var) override ;
};

class var_solver : public ctx_object {
	ast::value_term val;
public:
	var_solver(ast::value_term v);
	ast::value_term call(ast::function_call fnc) override ;
	ast::value_term solve(ast::var_name var) override ;
};

} // namespace cppjinja::evt
