/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "grammar/blocks.hpp"

#include "opterm.hpp"
#include "single.hpp"

namespace cppjinja::text {
	const auto block_free_text_term_def = lexeme[
		  block_term_start
		| op_term_start
		| comment_term_start
		| x3::eoi
	];
	const auto block_free_text_def = lexeme[
		   !block_free_text_term_def
		>> *(char_ >> !block_free_text_term_def)
		>> char_
	];

	const auto block_content_def =
	          block_free_text
	        | op_comment
	        | op_out
	        | op_set
	        | block_raw
	        | block_if
	        | block_for
	        | block_macro
	        | block_named
	        | block_filtered
	        | block_set
	        | block_call
	        ;
	const auto block_content_vec_def = +block_content;

	const auto block_term_end_cnt =
		lexeme[block_term_end >> -block_free_text];

	const auto block_raw_text_def =
		lexeme[+(char_ >> !block_term_start) >> char_];
	const auto block_raw_def =
		   block_term_start >> lit("raw")
		>> lexeme[block_term_end
		>> -block_raw_text >> block_term_start]
		>> lit("endraw") >> block_term_end;

	const auto block_if_def =
		   block_term_start >> omit[lit("if")] >> expr_ops::expr_bool
		>> block_term_end_cnt >> *block_content
		>> -else_thread >> block_term_start
		>> lit("endif") >> block_term_end
		;

	const auto else_thread_def =
		   block_term_start
		>> omit[lit("else")]
		>> block_term_end_cnt >> *block_content
		;

	const auto block_for_def =
		   block_term_start
		>> lit("for") >> (single_var_name % ',')
		>> lit("in") >> expr_ops::expr
		>> -(lit("recursive") >> x3::attr(true))
		>> block_term_end_cnt >> *block_content
		>> -else_thread
		>> block_term_start
		>> lit("endfor") >> block_term_end
		;

	const auto block_set_def =
		   block_term_start
		>> lit("set") >> single_var_name >> -('|' >> expr_ops::filter_call % '|')
		>> block_term_end_cnt >> *block_content >> block_term_start
		>> lit("endset") >> block_term_end
		;

	const auto macro_parameter_def = single_var_name >> -('=' >> expr_ops::expr);
	const auto macro_parameters_def =
		single_var_name >> -('(' >> -(macro_parameter % ',') >> ')');
	const auto block_macro_def =
		   block_term_start >> lit("macro") >> macro_parameters_def
		>> block_term_end_cnt >> *block_content >> block_term_start
		>> lexeme[lit("endmacro") >> -omit[+space >> single_var_name]]
		>> block_term_end
		;
	const auto block_named_def =
		   block_term_start >> lit("block") >> macro_parameters_def
		>> block_term_end_cnt >> *block_content >> block_term_start
		>> lit("endblock") >> -omit[+space >> single_var_name]
		>> block_term_end
		;
	const auto block_filtered_def =
	       block_term_start >> lit("filter") >> expr_ops::filter_call
	    >> block_term_end >> *block_content >> block_term_start
	    >> lit("endfilter") >> block_term_end
		;

	const auto call_parameter_def = -(single_var_name >> '=') >> expr_ops::expr;
	const auto block_call_def =
	       block_term_start
	    >> lit("call")
	    >> -(omit['('] >> (macro_parameter % ',') >> omit[')'])
	    >> single_var_name
	    >> -(omit['('] >> (call_parameter % ',') >> omit[')'])
	    >> block_term_end_cnt >> *block_content >> block_term_start
	    >> lit("endcall") >> block_term_end
	    ;

	class block_if_class          : x3::annotate_on_success {};
	class block_for_class         : x3::annotate_on_success {};
	class block_raw_class         : x3::annotate_on_success {};
	class block_set_class         : x3::annotate_on_success {};
	class block_call_class        : x3::annotate_on_success {};
	class else_thread_class       : x3::annotate_on_success {};
	class block_macro_class       : x3::annotate_on_success {};
	class block_named_class       : x3::annotate_on_success {};
	class block_filtered_class    : x3::annotate_on_success {};
	class block_raw_text_class    : x3::annotate_on_success {};
	class call_parameter_calss    : x3::annotate_on_success {};
	class block_free_text_class   : x3::annotate_on_success {};
	class macro_parameter_calss   : x3::annotate_on_success {};
	class block_content_vec_class : x3::annotate_on_success {};

	BOOST_SPIRIT_DEFINE( block_if )
	BOOST_SPIRIT_DEFINE( block_for )
	BOOST_SPIRIT_DEFINE( block_raw )
	BOOST_SPIRIT_DEFINE( block_set )
	BOOST_SPIRIT_DEFINE( block_call )
	BOOST_SPIRIT_DEFINE( else_thread )
	BOOST_SPIRIT_DEFINE( block_macro )
	BOOST_SPIRIT_DEFINE( block_named )
	BOOST_SPIRIT_DEFINE( block_content )
	BOOST_SPIRIT_DEFINE( block_filtered )
	BOOST_SPIRIT_DEFINE( block_raw_text )
	BOOST_SPIRIT_DEFINE( call_parameter )
	BOOST_SPIRIT_DEFINE( block_free_text )
	BOOST_SPIRIT_DEFINE( macro_parameter )
	BOOST_SPIRIT_DEFINE( block_content_vec )
} // namespace cppjinja::text

