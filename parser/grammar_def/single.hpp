/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "grammar/single.hpp"
#include "grammar/common.hpp"
#include "grammar/opterm.hpp"
#include "common.hpp"
#include "expr.hpp"

namespace cppjinja::text {

	auto const filter_call_def =  function_call | var_name;
	auto const op_out_def = op_term_start >> expr_ops::expr >> op_term_end;

	auto const op_comment_value_def = lexeme[*(char_ >> !comment_term_end) >> char_];
	auto const op_comment_def = comment_term_start >> !comment_term_end >> op_comment_value >> comment_term_end;

	auto const op_set_def = block_term_start >> lit("set") >> expr_ops::eq_assign >> block_term_end;

	auto const filename_def = quoted_string;
	auto const op_include_def =
	           block_term_start
	        >> omit[lit("include")]
	        >> filename
	        >> -(omit[lit("ignore") >> lit("missing")] >> x3::attr(true))
	        >> -((lit("with")    >> lit("context") >> x3::attr(true))
	           | (lit("without") >> lit("context") >> x3::attr(false)))
	        >> block_term_end
	        ;

	struct op_out_class      : error_handler, x3::annotate_on_success { };
	struct op_set_class      : error_handler, x3::annotate_on_success { };
	struct op_comment_class  : error_handler, x3::annotate_on_success { };
	struct op_include_class  : error_handler, x3::annotate_on_success { };
	struct filter_call_class : error_handler, x3::annotate_on_success { };
	struct filename_class    : error_handler, x3::annotate_on_success { };

	BOOST_SPIRIT_DEFINE( op_out )
	BOOST_SPIRIT_DEFINE( op_comment )
	BOOST_SPIRIT_DEFINE( op_set )
	BOOST_SPIRIT_DEFINE( op_comment_value )
	BOOST_SPIRIT_DEFINE( op_include )
	BOOST_SPIRIT_DEFINE( filter_call )
	BOOST_SPIRIT_DEFINE( filename )

} // namespace cppjinja::text

