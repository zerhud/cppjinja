/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/eval.hpp"
#include "parser/ast/common.hpp"
#include "evtree/declarations.hpp"

namespace cppjinja::evt {

class expr_filter final {
	const cppjinja::east::value_term& base;
	exenv* user_data;
	[[noreturn]] void ilegal_operator() const ;
public:
	expr_filter(exenv* ud, const cppjinja::east::value_term& b);

	east::value_term operator()(const ast::value_term& val) ;

	east::value_term operator()(const ast::var_name& obj) ;
	east::value_term operator()(const ast::function_call& obj) ;

	[[noreturn]] east::value_term operator()(const double& obj) const ;
	[[noreturn]] east::value_term operator()(const ast::string_t& obj) const ;
	[[noreturn]] east::value_term operator()(const ast::tuple_v& obj) const ;
	[[noreturn]] east::value_term operator()(const ast::array_v& obj) const ;
	[[noreturn]] east::value_term operator()(const ast::binary_op& op) const ;
};

} // namespace cppjinja::evt
