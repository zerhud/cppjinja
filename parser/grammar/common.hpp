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
	struct var_name_class;
	struct binary_op_class;
	struct function_call_parameter_class;
	struct function_call_class;
	struct value_term_class;

	const x3::rule<quoted_string_class, ast::string_t> quoted_string = "quoted_string";
	const x3::rule<var_name_class, ast::var_name> var_name = "var_name";
	const x3::rule<binary_op_class, ast::binary_op> binary_op = "binary_op";
	const x3::rule<function_call_parameter_class, ast::function_call_parameter> function_call_parameter = "function_call_parameter";
	const x3::rule<function_call_class, ast::function_call> function_call = "function_call";
	const x3::rule<value_term_class, ast::value_term> value_term = "value_term";

} // namespace cppjinja::text

