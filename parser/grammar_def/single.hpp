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

namespace cppjinja::text {

	auto const filter_call_def =  function_call | var_name;
	auto const op_out_def = op_term_start >> value_term >> -('|' >> filter_call % '|') >> op_term_end;

	auto const op_comment_value_def = lexeme[*(char_ >> !comment_term_end) >> char_];
	auto const op_comment_def = comment_term_start >> !comment_term_end >> op_comment_value >> comment_term_end;

	auto const op_set_def = block_term_start >> lit("set") >> var_name >> '=' >> value_term >> block_term_end;

	class op_out_class      : x3::annotate_on_success { };
	class op_set_class      : x3::annotate_on_success { };
	class op_comment_class  : x3::annotate_on_success { };
	class filter_call_class : x3::annotate_on_success { };

	BOOST_SPIRIT_DEFINE( op_out )
	BOOST_SPIRIT_DEFINE( op_comment )
	BOOST_SPIRIT_DEFINE( op_set )
	BOOST_SPIRIT_DEFINE( op_comment_value )
	BOOST_SPIRIT_DEFINE( filter_call )

} // namespace cppjinja::text

