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

	auto const op_out_def = op_term_start >> value_term >> op_term_end;
	auto const op_comment_def = comment_term_start >> -(*(char_ >> !comment_term_end) >> char_) >> comment_term_end;
	auto const op_set_def = op_term_start >> var_name >> '=' >> value_term >> op_term_end;

	BOOST_SPIRIT_DEFINE( op_out )
	BOOST_SPIRIT_DEFINE( op_comment )
	BOOST_SPIRIT_DEFINE( op_set )

} // namespace cppjinja::text

