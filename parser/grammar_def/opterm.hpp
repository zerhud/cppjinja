/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#ifndef PARSER_PRIVATE
#error this file is private file for parser2 library
#endif

#include "grammar/opterm.hpp"
#include "common.hpp"

namespace cppjinja::text {

	// ---
	// helpers function
	// ---
	constexpr auto get_data = [](auto& ctx){ return x3::get<parser_env>(ctx); };
	const auto check_if_term_start    = [](auto& ctx){
		_pass(ctx) = _attr(ctx) == get_data(ctx).output.b; };
	const auto check_if_term_end      = [](auto& ctx){
		_pass(ctx) = _attr(ctx) == get_data(ctx).output.e; };
	const auto check_if_block_start   = [](auto& ctx){
		_pass(ctx) = _attr(ctx) == get_data(ctx).term.b; };
	const auto check_if_block_end     = [](auto& ctx){
		_pass(ctx) = _attr(ctx) == get_data(ctx).term.e; };
	const auto check_if_comment_start = [](auto& ctx){
		_pass(ctx) = _attr(ctx) == get_data(ctx).cmt.b; };
	const auto check_if_comment_end   = [](auto& ctx){
		_pass(ctx) = _attr(ctx) == get_data(ctx).cmt.e; };
	const auto set_trim = [](auto& c){
		if constexpr (
			!std::is_same_v<
				 std::decay_t<decltype(_val(c))>
				,x3::unused_type
			>)
		_val(c).trim=true;
	};
	const auto set_bsign = [](auto& c){
		_val(c).bsign=_attr(c);
		if(_val(c).bsign && *_val(c).bsign < 0)
			_val(c).trim = true;
	};

	// ---
	// helpers expressions
	// ---
	const auto check_trim = -char_('+')[set_trim];
	const auto op_seq_def =
		x3::repeat(2)[char_("!#$%&()*,-./:;<=>?@[\\]^_`{|}~")];

	// ---
	// rules
	// ---
	const auto op_term_start_def =
		lexeme[op_seq_def[check_if_term_start] >> check_trim];
	const auto op_term_end_def   =
		lexeme[check_trim >> op_seq_def[check_if_term_end]];

	const auto block_term_start_def =
		lexeme[op_seq_def[check_if_block_start] >> check_trim >> -x3::int_[set_bsign]];
	const auto block_term_end_def   =
		lexeme[check_trim >> -x3::int_[set_bsign] >> op_seq_def[check_if_block_end]];

	const auto comment_term_start_def =
		lexeme[op_seq_def[check_if_comment_start] >> check_trim];
	const auto comment_term_end_def   =
		lexeme[check_trim >> op_seq_def[check_if_comment_end]];

	// ---
	// tags
	// ---
	class op_term_start_class      : x3::annotate_on_success { };
	class op_term_end_class        : x3::annotate_on_success { };
	class block_term_start_class   : x3::annotate_on_success { };
	class block_term_end_class     : x3::annotate_on_success { };
	class comment_term_start_class : x3::annotate_on_success { };
	class comment_term_end_class   : x3::annotate_on_success { };

	// ---
	// definitions
	// ---
	BOOST_SPIRIT_DEFINE( op_term_start )
	BOOST_SPIRIT_DEFINE( op_term_end )
	BOOST_SPIRIT_DEFINE( block_term_start )
	BOOST_SPIRIT_DEFINE( block_term_end )
	BOOST_SPIRIT_DEFINE( comment_term_start )
	BOOST_SPIRIT_DEFINE( comment_term_end )

} // namespace cppjinja::text
