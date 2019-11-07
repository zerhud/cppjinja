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

namespace cppjinja::text {
	const auto block_free_text_def = +(char_ >> !block_term_start) >> char_;
	const auto block_raw_text_def = +(char_ >> !block_term_start) >> char_;
	const auto block_raw_def =
		   block_term_start >> omit[lit("raw")] >> block_term_end
		>> -block_raw_text
		>> block_term_start >> lit("endraw") >> block_term_end;

	class block_free_text_class : x3::annotate_on_success {};
	class block_raw_text_class : x3::annotate_on_success {};
	class block_raw_class : x3::annotate_on_success {};

	BOOST_SPIRIT_DEFINE( block_free_text )
	BOOST_SPIRIT_DEFINE( block_raw_text )
	BOOST_SPIRIT_DEFINE( block_raw )
} // namespace cppjinja::text

