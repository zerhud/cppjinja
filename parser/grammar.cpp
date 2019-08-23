/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "grammar.hpp"
#include "traits.hpp"
#include "ast.hpp"

namespace cppjinja {


namespace grammar::utf8 {
using gram_traits = traits::unicode;

#include "grammar.ipp"

} // namespace grammar::utf8

template<typename StringTraits>
auto inner_parse(const typename StringTraits::String& data)
{
	auto begin = data.begin();
	auto end = data.end();

	bool ok = false;
	b_block result;

	try {
		grammar<StringTraits> parser;
		ok = boost::spirit::qi::phrase_parse(begin,end, parser, StringTraits::space, result);
	}
	catch(qi::expectation_failure<const char*>& ex) {
		(void) ex;
		throw;
	}

	if(!ok) throw std::runtime_error("cannot parse");
	return  result;
}

} // namespace cppjinja

cppjinja::b_block<std::string> cppjinja::parse(std::string_view data)
{
	return cppjinja::inner_parse<string_view_traits>(data);
}

cppjinja::b_block<std::wstring> cppjinja::parse(std::wstring_view data)
{
	return cppjinja::inner_parse<wstring_view_traits>(data);
}

