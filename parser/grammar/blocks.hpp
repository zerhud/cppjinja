/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/blocks.hpp"

namespace cppjinja::text {
	namespace x3 = boost::spirit::x3;

	class block_raw_class;

	const x3::rule<block_raw_class, ast::block_raw> block_raw = "block_raw";

	BOOST_SPIRIT_DECLARE(decltype(block_raw))

} // namespace cppjinja::text

