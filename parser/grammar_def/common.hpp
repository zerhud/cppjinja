/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

//#define BOOST_SPIRIT_X3_UNICODE

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include "grammar/common.hpp"

namespace cppjinja::text {
	//auto& char_ = x3::unicode::char_;
	//auto& space = x3::unicode::space;
	//using boost::spirit::x3::standard_wide::lit;

	auto& char_ = x3::char_;
	auto& space = x3::space;
	using boost::spirit::x3::lit;
	using boost::spirit::x3::skip;

	auto const quoted_string_1_def = *(char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> char_;
	auto const quoted_string_2_def = *(char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> char_;
	auto const quoted_string_def = lit("'") >> -quoted_string_1_def >> lit("'");

	auto const single_var_name_def = char_("A-Za-z_") >> *char_("0-9A-Za-z_");

	auto const var_name_def = single_var_name % '.';

	auto const binary_op_sign =
		  lit("==") >> x3::attr(ast::comparator::eq)
		| lit("is") >> x3::attr(ast::comparator::eq)
		| lit("!=") >> x3::attr(ast::comparator::neq)
		| lit("<=") >> x3::attr(ast::comparator::less_eq)
		| lit(">=") >> x3::attr(ast::comparator::more_eq)
		| lit( "<") >> x3::attr(ast::comparator::less)
		| lit( ">") >> x3::attr(ast::comparator::more)
	;

	auto const binary_op1_def = value_term_r2 >> binary_op_sign >> value_term_r2;
	auto const binary_op2_def = value_term_r1 >> binary_op_sign >> value_term_r1;

	auto const value_term_r1_def = quoted_string | var_name | binary_op2 | function_call;
	auto const value_term_r2_def = quoted_string | var_name | binary_op1 | function_call;

	auto const function_call_parameter_def = skip(space)[-(single_var_name >> '=') >> value_term_r1];
	auto const function_call_def =
		skip(space)[
			var_name >> x3::omit['('] >> -(!char_(')') >> function_call_parameter % ',') >> x3::omit[')']
		];

	BOOST_SPIRIT_DEFINE( quoted_string )
	BOOST_SPIRIT_DEFINE( single_var_name )
	BOOST_SPIRIT_DEFINE( var_name )
	BOOST_SPIRIT_DEFINE( binary_op1 )
	BOOST_SPIRIT_DEFINE( binary_op2 )
	BOOST_SPIRIT_DEFINE( value_term_r1 )
	BOOST_SPIRIT_DEFINE( value_term_r2 )
	BOOST_SPIRIT_DEFINE( function_call )
	BOOST_SPIRIT_DEFINE( function_call_parameter )

	class quoted_string_class : x3::annotate_on_success {};
	class single_var_name_class : x3::annotate_on_success {};
	class var_name_class : x3::annotate_on_success {};
	class binary_op1_class : x3::annotate_on_success {};
	class binary_op2_class : x3::annotate_on_success {};
	class value_term_r1_class : x3::annotate_on_success {};
	class value_term_r2_class : x3::annotate_on_success {};
	class function_call_class : x3::annotate_on_success {};
	class function_call_parameter_class : x3::annotate_on_success {};

} // namespace cppjinja::text
