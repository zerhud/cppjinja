/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_SPIRIT_X3_UNICODE

#include <boost/phoenix.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>

namespace x3 = boost::spirit::x3;
namespace pl = std::placeholders;

namespace cppjinja::traits {

struct ascii {
	struct types {
		using out_string_t = std::string;
		using input_iterator_t = std::string_view::iterator;

		using space_t = x3::ascii::space_type;
		using char_t = x3::ascii::char_;
		using lit_t = x3::ascii::lit_t;
	};

	static decltype(x3::ascii::space)& space;
	static decltype(x3::ascii::char_)& char_;

	template<typename InStr>
	static void concat(out_string_t& result, InStr in)
	{
		result += boost::spirit::to_utf8(in);
	}
};
struct unicode {
	struct types {
		using out_string_t = std::string;
		using input_iterator_t = std::string_view::iterator;

		using space_t = x3::unicode::space_type;
		using char_t = x3::unicode::char_;
		using lit_t = x3::unicode::lit_t;
	};

	static decltype(x3::unicode::space)& space;
	static decltype(x3::unicode::char_)& char_;

	template<typename InStr>
	static void concat(out_string_t& result, InStr in)
	{
		result += boost::spirit::to_utf8(in);
	}
};
decltype(ascii::space)& ascii::space = x3::ascii::space;
decltype(ascii::char_)& ascii::char_ = x3::ascii::char_;
decltype(unicode::space)& unicode::space = x3::unicode::space;
decltype(unicode::char_)& unicode::char_ = x3::unicode::char_;

} // namespace cppjinja::traits
