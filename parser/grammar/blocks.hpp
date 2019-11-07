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

	class block_free_text_class;
	class block_raw_text_class;
	class block_raw_class;
	class block_if_class;

	const x3::rule<block_free_text_class, std::string> block_free_text = "block_free_text";

	const x3::rule<block_raw_text_class, std::string> block_raw_text = "block_raw_text";
	const x3::rule<block_raw_class, ast::block_raw> block_raw = "block_raw";

	const x3::rule<block_if_class, ast::block_if> block_if = "block_if";

	BOOST_SPIRIT_DECLARE(decltype(block_free_text))
	BOOST_SPIRIT_DECLARE(decltype(block_raw_text))
	BOOST_SPIRIT_DECLARE(decltype(block_raw))
	BOOST_SPIRIT_DECLARE(decltype(block_if))

} // namespace cppjinja::text

