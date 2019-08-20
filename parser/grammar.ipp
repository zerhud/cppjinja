/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#define BOOST_SPIRIT_X3_UNICODE

#include <boost/phoenix.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>

#include "ast.hpp"

namespace x3 = boost::spirit::x3;
namespace pl = std::placeholders;

namespace cppjinja {

namespace traits {
struct ascii {
	using out_string_t = std::string;
	static decltype(x3::ascii::space)& space;
	static decltype(x3::ascii::char_)& char_;
};
struct unicode {
	using out_string_t = std::string;
	static decltype(x3::unicode::space)& space;
	static decltype(x3::unicode::char_)& char_;
};
decltype(ascii::space)& ascii::space = x3::ascii::space;
decltype(ascii::char_)& ascii::char_ = x3::ascii::char_;
decltype(unicode::space)& unicode::space = x3::unicode::space;
decltype(unicode::char_)& unicode::char_ = x3::unicode::char_;
} // namespace traits

namespace parser_tmpl {
	template<typename Traits>
	using out_string_t = Traits::string_t;

	template<typename OutString>
	const x3::rule<struct out_string, OutString> string = "string";
	template<typename Traits>
	const auto string_def = x3::lexeme[ *
} // namespace parser_tmpl

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

