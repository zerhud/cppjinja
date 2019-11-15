/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "grammar/tmpls.hpp"
#include "grammar/opterm.hpp"
#include "grammar/single.hpp"

namespace cppjinja::text {
	auto const extend_st_def =
		  block_term_start
		>> lit("extends")
		>> -( lit("template") >> quoted_string >> lit("from") )
		>> filename
		>> block_term_end
		;

	class extend_st_class : x3::annotate_on_success {};

	BOOST_SPIRIT_DEFINE( extend_st )
} // namespace cppjinja::text

