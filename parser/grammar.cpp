/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "grammar.hpp"

#define BOOST_SPIRIT_UNICODE

#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/home/support/char_encoding/unicode.hpp>

namespace qi = boost::spirit::qi;
namespace pl = std::placeholders;
namespace encoding = boost::spirit::unicode;

namespace cppjinja {

template<typename Iterator, typename Space>
struct grammar : qi::grammar<Iterator, b_block(), Space> {
	grammar() : grammar::base_type(rule_block, "block")
	{
		using qi::_val;
		using qi::_1;

		using boost::phoenix::at_c;
		using boost::phoenix::push_back;

		rule_block.name("block");
		rule_block = rule_content [push_back(at_c<1>(_val), _1)];

		rule_content.name("content");
		rule_content = qi::lexeme[ *(~encoding::char_('<')) [_val+=_1] ];
	}

	qi::rule<Iterator, b_block(), Space> rule_block;
	qi::rule<Iterator, std::string(), Space> rule_content;
};

template<typename String>
auto inner_parse(String data)
{
	auto begin = data.begin();
	auto end = data.end();

	bool ok = false;
	b_block result;

	try {
		grammar<decltype(begin), encoding::space_type> parser;
		ok = boost::spirit::qi::phrase_parse(begin,end, parser, encoding::space, result);
	}
	catch(qi::expectation_failure<const char*>& ex) {
		(void) ex;
		throw;
	}

	if(!ok) throw std::runtime_error("cannot parse");
	return  result;
}

} // namespace cppjinja

cppjinja::b_block cppjinja::parse(std::string_view data)
{
	return cppjinja::inner_parse(data);
}

cppjinja::b_block cppjinja::parse(std::wstring_view data)
{
	return cppjinja::inner_parse(data);
}
