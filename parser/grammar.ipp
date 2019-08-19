/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#define BOOST_SPIRIT_UNICODE

#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/home/support/char_encoding/unicode.hpp>

#include "ast.hpp"

namespace qi = boost::spirit::qi;
namespace pl = std::placeholders;

namespace cppjinja {


template<typename StringTraits>
struct grammar : qi::grammar<typename StringTraits::iterator_t, b_block<typename StringTraits::string_t>(), typename StringTraits::space_t> {
	grammar() : grammar::base_type(rule_block, "block")
	{
		using qi::_val;
		using qi::_1;

		using boost::phoenix::at_c;
		using boost::phoenix::push_back;

		rule_block.name("block");
		rule_block = rule_content [push_back(at_c<1>(_val), _1)];

		rule_content.name("content");
		rule_content = qi::lexeme[ *(~StringTraits::char_('<')) [_val+=_1] ];
	}

	qi::rule<typename StringTraits::iterator_t, b_block<typename StringTraits::string_t>(), typename StringTraits::space_t> rule_block;
	qi::rule<typename StringTraits::iterator_t, typename StringTraits::string_t(), typename StringTraits::space_t> rule_content;
};

} // namespace cppjinja

