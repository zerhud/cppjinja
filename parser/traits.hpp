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
#include "ast.hpp"

namespace x3 = boost::spirit::x3;
namespace pl = std::placeholders;

namespace cppjinja::traits {

namespace helpers {
template<class T>struct tag_t{};
template<class T>constexpr tag_t<T> tag{};
namespace detect_string {
	template<class T, class...Ts>
	constexpr bool is_stringlike(tag_t<T>, Ts&&...){ return false; }
	template<class T, class A>
	constexpr bool is_stringlike( tag_t<std::basic_string<T,A>> ){ return true; }
	template<class T>
	constexpr bool detect=is_stringlike(tag<T>); // enable ADL extension
}
namespace detect_character {
	template<class T, class...Ts>
	constexpr bool is_charlike(tag_t<T>, Ts&&...){ return false; }
	constexpr bool is_charlike( tag_t<char> ){ return true; }
	constexpr bool is_charlike( tag_t<wchar_t> ){ return true; }
	// ETC
	template<class T>
	constexpr bool detect=is_charlike(tag<T>); // enable ADL extension
}
} // namespace helpers

struct common {
	template<typename T, typename String>
	static
	std::enable_if_t<helpers::detect_string::detect<std::decay_t<String>>>
	emplace_back_utf8(std::vector<T>& vec, String&& v)
	{
		vec.emplace_back(boost::spirit::to_utf8(v));
	}

	template<typename T, typename In>
	static
	std::enable_if_t<!(helpers::detect_string::detect<std::decay_t<In>>)>
	emplace_back_utf8(std::vector<T>& vec, In&& v)
	{
		vec.emplace_back(std::forward<In>(v));
	}
};

struct ascii : common {
	struct types {
		using out_string_t = std::string;
		using input_iterator_t = std::string_view::iterator;

		using space_t = x3::ascii::space_type;
		//using char_t = x3::ascii::char_;
		//using lit_t = x3::ascii::lit;

		using block_t = b_block<out_string_t>;
	};

	//static decltype(x3::ascii::lit)& lit;
	static decltype(x3::ascii::char_)& char_;
	static decltype(x3::ascii::space)& space;

	template<typename InStr>
	static void concat(types::out_string_t& result, InStr in)
	{
		result += boost::spirit::to_utf8(in);
	}

	template<typename InStr>
	static bool compare(const InStr& left, const std::string& right)
	{
		return left == right;
	}
};
struct unicode_utf8 : common {
	struct types {
		using out_string_t = std::string;
		using input_iterator_t = std::string_view::iterator;

		using space_t = x3::unicode::space_type;
		//using char_t = x3::unicode::char_;
		//using lit_t = x3::unicode::lit_t;

		using block_t = b_block<out_string_t>;
		using op_out_t = st_out<out_string_t>;
	};

	//static decltype(x3::ascii::lit)& lit;
	static decltype(x3::unicode::space)& space;
	static decltype(x3::unicode::char_)& char_;

	template<typename InStr>
	static void concat(types::out_string_t& result, InStr in)
	{
		result += boost::spirit::to_utf8(in);
	}

	template<typename InStr>
	static bool compare(const InStr& left, const std::string& right)
	{
		return left == right;
	}
};
struct unicode_wide : common {
	struct types {
		using out_string_t = std::wstring;
		using input_iterator_t = std::string_view::iterator;

		using space_t = x3::unicode::space_type;
		//using char_t = x3::unicode::char_;
		//using lit_t = x3::unicode::lit_t;

		using block_t = b_block<out_string_t>;
		using op_out_t = st_out<out_string_t>;
	};

	//static decltype(x3::ascii::lit)& lit;
	static decltype(x3::unicode::space)& space;
	static decltype(x3::unicode::char_)& char_;

	template<typename InStr>
	static void concat(types::out_string_t& result, InStr in)
	{
		result += in;
	}

	template<typename InStr>
	static bool compare(const InStr& left, const std::string& right)
	{
		return boost::spirit::to_utf8(left) == right;
	}
};
decltype(ascii::space)& ascii::space = x3::ascii::space;
decltype(ascii::char_)& ascii::char_ = x3::ascii::char_;
decltype(unicode_utf8::space)& unicode_utf8::space = x3::unicode::space;
decltype(unicode_utf8::char_)& unicode_utf8::char_ = x3::unicode::char_;
decltype(unicode_wide::space)& unicode_wide::space = x3::unicode::space;
decltype(unicode_wide::char_)& unicode_wide::char_ = x3::unicode::char_;

} // namespace cppjinja::traits