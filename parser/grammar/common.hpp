/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/common.hpp"

namespace cppjinja::text {
	namespace x3 = boost::spirit::x3;

	struct quoted_string_class;
	struct single_var_name_class;
	struct var_name_class;
	struct binary_op1_class;
	struct binary_op2_class;
	struct function_call_parameter_class;
	struct function_call_class;
	struct value_term_r1_class;
	struct value_term_r2_class;

	const x3::rule<quoted_string_class, ast::string_t> quoted_string = "quoted_string";
	const x3::rule<single_var_name_class, ast::string_t> single_var_name = "single_var_name";
	const x3::rule<var_name_class, ast::var_name> var_name = "var_name";
	const x3::rule<binary_op1_class, ast::binary_op> binary_op1 = "binary_op1";
	const x3::rule<binary_op2_class, ast::binary_op> binary_op2 = "binary_op2";
	const x3::rule<function_call_parameter_class, ast::function_call_parameter> function_call_parameter = "function_call_parameter";
	const x3::rule<function_call_class, ast::function_call> function_call = "function_call";
	const x3::rule<value_term_r1_class, ast::value_term> value_term_r1 = "value_term_r1";
	const x3::rule<value_term_r2_class, ast::value_term> value_term_r2 = "value_term_r2";

} // namespace cppjinja::text

