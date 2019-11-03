/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>

namespace cppjinja::text {
	namespace x3 = boost::spirit::x3;
	using iterator_type = std::string_view::const_iterator;
	using context_type = x3::phrase_parse_context<x3::ascii::space_type>::type;
} // namespace cppjinja::text
