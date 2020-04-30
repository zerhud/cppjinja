/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include "grammar/common.hpp"
#include "grammar/opterm.hpp"

namespace cppjinja::text {
	//auto& char_ = x3::unicode::char_;
	//auto& space = x3::unicode::space;
	//using boost::spirit::x3::standard_wide::lit;

	extern decltype(x3::char_)& char_;
	extern decltype(x3::space)& space;
	using boost::spirit::x3::lit;
	using boost::spirit::x3::skip;
	using boost::spirit::x3::omit;
	using boost::spirit::x3::lexeme;

	auto const quoted_string_1_def = *(char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> char_;
	auto const quoted_string_2_def = *(char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> char_;
	auto const quoted_string_def = lexeme[lit("'") >> -quoted_string_1_def >> lit("'")];

	auto const single_var_name_def = lexeme[char_("A-Za-z_") >> *char_("0-9A-Za-z_")];

	auto const array_v_def = lit('[') >> (value_term % ',') >> lit(']');
	auto const tuple_v_def = lit('(') >> (value_term % ',') >> lit(')');

	auto const var_name_def = single_var_name >> *('.' >> single_var_name % '.');
	auto const array_call_def = var_name >> '[' >> value_term >> ']';

	auto const binary_op_sign =
		  lit("in") >> x3::attr(ast::comparator::in)
		| lit("==") >> x3::attr(ast::comparator::eq)
		| lit("is") >> x3::attr(ast::comparator::eq)
		| lit("!=") >> x3::attr(ast::comparator::neq)
		| lit("<=") >> x3::attr(ast::comparator::less_eq)
		| lit(">=") >> x3::attr(ast::comparator::more_eq)
		| lit( "<") >> x3::attr(ast::comparator::less)
		| lit( ">") >> x3::attr(ast::comparator::more)
	;

	auto const binary_op_def = value_term >> binary_op_sign >> value_term;

	auto const value_term_def =
	        !block_term_start >>
	              ( x3::double_
	              | array_v
	              | tuple_v
	              | function_call
	              | quoted_string
	              | array_call
	              | var_name
	              | binary_op
	              );

	auto const function_call_parameter_def = -(single_var_name >> '=') >> value_term;
	auto const function_call_def =
			var_name >> x3::omit['('] >> -(!char_(')') >> function_call_parameter % ',') >> x3::omit[')']
		;

	class quoted_string_class   : x3::annotate_on_success {};
	class single_var_name_class : x3::annotate_on_success {};
	class var_name_class        : x3::annotate_on_success {};
	class binary_op_class       : x3::annotate_on_success {};
	class value_term_class      : x3::annotate_on_success {};
	class function_call_class   : x3::annotate_on_success {};
	class function_call_parameter_class : x3::annotate_on_success {};
	class array_class : x3::annotate_on_success {};
	class tuple_class : x3::annotate_on_success {};

	BOOST_SPIRIT_DEFINE( quoted_string )
	BOOST_SPIRIT_DEFINE( single_var_name )
	BOOST_SPIRIT_DEFINE( var_name )
	BOOST_SPIRIT_DEFINE( binary_op )
	BOOST_SPIRIT_DEFINE( value_term )
	BOOST_SPIRIT_DEFINE( function_call )
	BOOST_SPIRIT_DEFINE( function_call_parameter )
	BOOST_SPIRIT_DEFINE( array_v )
	BOOST_SPIRIT_DEFINE( tuple_v )
	BOOST_SPIRIT_DEFINE( array_call )

} // namespace cppjinja::text
