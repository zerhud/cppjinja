/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "grammar/opterm.hpp"
#include "common.hpp"

namespace cppjinja::text {

    auto get_data = [](auto& ctx){ return x3::get<parser_data>(ctx); };

	const auto op_seq_def = x3::repeat(2)[char_("!#$%&()*,-./:;<=>?@[\\]^_`{|}~")];
	const auto op_term_start_def = op_seq_def[([](auto&c){_pass(c)=_attr(c)==get_data(c).output.b;})] >> -(omit['+'] >> x3::attr(true));

	const auto op_term_end_def = "op_term_start";
	const auto block_term_start_def = "block_term_start";
	const auto block_term_end_def = "block_term_start";

	BOOST_SPIRIT_DEFINE( op_term_start )

	class op_term_start_class : x3::annotate_on_success {};

} // namespace cppjinja::text
