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

	class quoted_string_class;
	class single_var_name_class;
	class var_name_class;
	class binary_op_class;
	class function_call_parameter_class;
	class function_call_class;
	class value_term_class;

	const x3::rule<quoted_string_class, ast::string_t> quoted_string = "quoted_string";
	const x3::rule<single_var_name_class, ast::string_t> single_var_name = "single_var_name";
	const x3::rule<var_name_class, ast::var_name> var_name = "var_name";
	const x3::rule<binary_op_class, ast::binary_op> binary_op = "binary_op";
	const x3::rule<function_call_parameter_class, ast::function_call_parameter> function_call_parameter = "function_call_parameter";
	const x3::rule<function_call_class, ast::function_call> function_call = "function_call";
	const x3::rule<value_term_class, ast::value_term> value_term = "value_term";

	BOOST_SPIRIT_DECLARE(decltype(quoted_string))
	BOOST_SPIRIT_DECLARE(decltype(single_var_name))
	BOOST_SPIRIT_DECLARE(decltype(var_name))
	BOOST_SPIRIT_DECLARE(decltype(binary_op))
	BOOST_SPIRIT_DECLARE(decltype(function_call_parameter))
	BOOST_SPIRIT_DECLARE(decltype(function_call))
	BOOST_SPIRIT_DECLARE(decltype(value_term))

} // namespace cppjinja::text

