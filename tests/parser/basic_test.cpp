/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cppjinja parser

#include <bitset>
#include <sstream>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/grammar.hpp"
#include "parser/debug_operators.hpp"

namespace std { 
	inline std::ostream& operator<<(std::ostream& out, const std::wstring& value) 
	{ 
		(void)value;
		out << "[wstring value, see " << __FILE__ << ':' << __LINE__ << ']';
		return out; 
	} 
	template<typename A, typename B>
	std::ostream& operator<<(std::ostream& out, const std::pair<A,B>& p)
	{
		out << p.first << ", " << p.second;
		return out;
	}
	template<typename A, typename B>
	std::ostream& operator<<(std::ostream& out, const std::pair<A,std::vector<B>>& p)
	{
		out << p.first << ": ";
		for(auto& i:p.second) out << i << ',';
		return out;
	}
	template<typename T>
	std::ostream& operator << (std::ostream& out, const std::vector<T>& p)
	{
		out << '[';
		for(auto& i:p) out << i << ',';
		out << ']';
		return out;
	}

} // namespace std 

namespace tests {
const std::string& rnd_data_string()
{
	static std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()-=+/\\?}{[]";
	return alphabet;
}

const std::string& rnd_data_alnum()
{
	static std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	return alphabet;
}

std::string gen_random_string(const std::string& alphabet)
{
	std::random_device rdev;
	std::mt19937 gen(rdev());
	std::uniform_int_distribution dis(0, static_cast<int>(alphabet.size()-1));

	std::string ret;
	std::int64_t size = dis(gen);
	while(size--) ret += alphabet.at(static_cast<std::size_t>(dis(gen)));

	return ret;
}

std::string random_string() { return gen_random_string(rnd_data_string()); }
std::string random_alnum() { return gen_random_string(rnd_data_alnum()); }

typedef std::bitset<2> mbflags;
mbflags make_mbflags(bool no_cnt=false, bool emb=false)
{
	mbflags ret;
	if(no_cnt) ret.set(0);
	if(emb) ret.set(1);
	return ret;
}

template<typename String, typename Ref, typename... Cnt>
auto make_block(mbflags flags, Ref&& ref, Cnt&&... cnt)
{
	using block_t = cppjinja::b_block<String>;
	using cnt_t = cppjinja::block_content<String>;
	auto ret = block_t{std::forward<Ref>(ref), cnt_t{std::forward<Cnt>(cnt)...}};
	if(flags.test(0)) ret.cnt.clear();
	if(flags.test(1)) ret = block_t{std::nullopt, cnt_t{std::move(ret)}};
	return ret;
}

template<typename Ref, typename... Cnt>
auto make_sblock(mbflags flags, Ref&& ref, Cnt&&... cnt)
{
	//return make_block<std::string>(false, std::nullopt, make_block<std::string>(no_cnt, std::forward<Ref>(ref), std::forward<Cnt>(cnt)...));
	return make_block<std::string>(flags, std::forward<Ref>(ref), std::forward<Cnt>(cnt)...);
}

template<typename Ref, typename... Cnt>
auto make_wblock(mbflags flags, Ref&& ref, Cnt&&... cnt)
{
	//return make_block<std::wstring>(false, make_block<std::wstring>(no_cnt, std::forward<Ref>(ref), std::forward<Cnt>(cnt)...));
	return make_block<std::wstring>(flags, std::forward<Ref>(ref), std::forward<Cnt>(cnt)...);
}

} // namespace tests

using namespace std::literals;
using cppjinja::parse;
namespace ut = boost::unit_test;

template<typename Obj, typename Val>
void check_block(const Obj& obj, std::size_t ind, const Val& val)
{
	BOOST_REQUIRE_LE(ind+1, obj.cnt.size());
	BOOST_TEST_CONTEXT("Object [" << ind << "] from " << obj)
	    BOOST_REQUIRE(std::holds_alternative<Val>(obj.cnt[ind]));
	BOOST_CHECK_EQUAL(std::get<Val>(obj.cnt[ind]), val);
}

template<typename Obj, typename Val, typename... Vals>
void check_block(const Obj& obj, std::size_t ind, const Val& val, const Vals&... vals)
{
	check_block(obj, ind, val);
	check_block(obj, ind+1, vals...);
}

template<typename Block, typename Val, typename... Vals>
void compare_block(const Block& left, std::size_t ind, const Val& val, const Vals&... vals)
{
	//BOOST_REQUIRE_LE(ind+1, left.cnt.size());
	//BOOST_TEST(left.cnt[ind] == val);
	BOOST_TEST(left == val);

	if constexpr ( 0 < sizeof...(vals) ) compare_block(left, ind+1, vals...);
}

template<typename Vals>
void parse_check_blocks(std::string_view data, std::size_t ind, const Vals& vals)
{
	BOOST_TEST_CONTEXT("Data was " << data) {
		cppjinja::s_jtmpl tmpl;
		BOOST_CHECK_NO_THROW(tmpl=cppjinja::parse(data));
		BOOST_TEST( tmpl == vals );
		//compare_block(tmpl, ind, vals...);
	}
}

template<typename... Vals>
void parse_check_content(std::string_view data, std::size_t ind, Vals&&... vals)
{
	parse_check_blocks(data, ind, tests::make_sblock(tests::make_mbflags(), std::nullopt, std::forward<Vals>(vals)...));
}

template<typename Vals>
void wparse_check_blocks(std::wstring_view data, std::size_t ind, const Vals& vals)
{
	BOOST_TEST_CONTEXT("Data was wchar_t* string...") {
		cppjinja::w_jtmpl tmpl;
		BOOST_CHECK_NO_THROW(tmpl=cppjinja::wparse(data));
		//compare_block(tmpl, ind, vals...);
		BOOST_TEST( tmpl==vals );
	}
}

template<typename... Vals>
void wparse_check_content(std::wstring_view data, std::size_t ind, Vals&&... vals)
{
	using cnt_t = cppjinja::block_content<std::wstring>;
	wparse_check_blocks(data, ind, cppjinja::w_block{std::nullopt, cnt_t{std::forward<Vals>(vals)...}});
}

BOOST_DATA_TEST_CASE(just_str
		,   ut::data::make("test string"s, "привет 你好，世界！"s)
		  ^ ut::data::make(L"test string"s, L"привет"s)
		, data, wdata)
{
	BOOST_TEST_CONTEXT("test with wide string, line " << __LINE__) {
		parse_check_content(data, 0, data);
		wparse_check_content(wdata, 0, wdata);
	}
}

BOOST_AUTO_TEST_SUITE(output)
BOOST_AUTO_TEST_CASE(string)
{
	parse_check_content("test <= 'kuku' =>"sv, 0, "test "s, cppjinja::st_out<std::string>{ "kuku"s, {} });
	parse_check_content("test <= \"kuku\" =>"sv, 0, "test "s, cppjinja::st_out<std::string>{ "kuku"s, {} });
	parse_check_content("<= 'kuk\\'u' =>"sv, 0, cppjinja::st_out<std::string>{ "kuk'u"s, {} });

	BOOST_TEST_CONTEXT("Russian data, line " << __LINE__ ) {
		wparse_check_content(L"<= 'привет' =>", 0, cppjinja::st_out<std::wstring>{ L"привет"s, {} });
		wparse_check_content(L"п<= 'привет' =>", 0, L"п"s, cppjinja::st_out<std::wstring>{ L"привет"s, {} });
	}
}
BOOST_DATA_TEST_CASE(variable
		,   ut::data::make("<= var =>"sv, "<= foo.bar =>"sv)
		  ^ ut::data::make(cppjinja::var_name{"var"}, cppjinja::var_name{"foo", "bar"})
		, data, result)
{
	using cppjinja::parse;
	using sto_t = cppjinja::st_out<std::string>;

	parse_check_content(data, 0, sto_t{ result, {} } );
	parse_check_content(data, 0, sto_t{ result, {} } );
}
BOOST_AUTO_TEST_CASE(function)
{
	using cppjinja::parse;
	using cppjinja::var_name;
	using sto_t = cppjinja::st_out<std::string>;
	using fnc_call = cppjinja::fnc_call<std::string>;

	parse_check_content("q<= foo() =>"sv, 0, "q"s, sto_t{ fnc_call{{"foo"s}, {}}, {} });
	parse_check_content("<= foo(qq) =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"qq"s}}}, {} });
	parse_check_content("<= foo(q.q) =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"q"s, "q"s}}}, {} });
	parse_check_content("<= foo(qq, q.q) =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"qq"s}, var_name{"q"s, "q"s}}}, {} });
	parse_check_content("<= foo('qq') =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {"qq"s}}, {} });
}
BOOST_AUTO_TEST_CASE(filters)
{
	using cppjinja::parse;
	using cppjinja::var_name;
	using sto_t = cppjinja::st_out<std::string>;
	using fnc_call = cppjinja::fnc_call<std::string>;

	parse_check_content("<= foo() | filter =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {}}, {var_name{"filter"s}} });
	parse_check_content("<= foo(qq)|f1|f2=>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"qq"s}}}, {var_name{"f1"s},var_name{"f2"s}} });
	parse_check_content("text<= 'q'|f =>"sv, 0, "text"s, sto_t{ "q"s, {var_name{"f"s}} });
	parse_check_content("<= 'q'|f() =>"sv, 0, sto_t{ "q"s, {fnc_call{{"f"s},{}}} });
	parse_check_content("<= 'q'|f('q') =>"sv, 0, sto_t{ "q"s, {fnc_call{{"f"s},{"q"s}}} });
	parse_check_content("<= 'q'|a.f('q',q) =>"sv, 0, sto_t{ "q"s, {fnc_call{{"a"s,"f"s},{"q"s,var_name{"q"s}}}} });
}
BOOST_AUTO_TEST_SUITE_END() // output
BOOST_AUTO_TEST_SUITE(blocks)
BOOST_AUTO_TEST_SUITE(op_if)
BOOST_DATA_TEST_CASE(
	  simple
	,   ut::data::make("kuku"s, ""s, "a"s, "aa"s, "abc"s)
	  * ut::data::make("if a is b"s, " if a is b"s, " if a is b "s)
	  * ut::data::make("endif"s, "endif "s, " endif "s)
	, text, start, finish )
{
	using cppjinja::s_block;
	using cppjinja::comparator;
	using cppjinja::var_name;
	using st_if = cppjinja::st_if<std::string>;

	std::string data = "<%"s + start + "%>"s + text + "<%"s + finish + "%>"s;
	auto flags = tests::make_mbflags(text.empty(), true);
	auto result = tests::make_sblock(flags, st_if{comparator::eq, var_name{"a"s}, var_name{"b"s}}, text);

	parse_check_blocks(data, 0, result);
}
BOOST_DATA_TEST_CASE(
	  errors
	,   ut::data::make("kuku"s, ""s, "a"s, "aa"s, "abc"s)
	  * ut::data::make("if a is b"s, " if a is"s)
	  * ut::data::make(""s, "kuku"s, " endfor "s)
	, text, start, finish )
{
	std::string data = "<%"s + start + "%>"s + text + "<%"s + finish + "%>"s;
	BOOST_CHECK_THROW(cppjinja::parse(data), std::exception);
}
BOOST_AUTO_TEST_CASE(no_comparator)
{
	using cppjinja::s_block;
	using cppjinja::comparator;
	using cppjinja::var_name;
	using st_if = cppjinja::st_if<std::string>;

	auto flags = tests::make_mbflags(false, true);
	auto result = tests::make_sblock(flags, st_if{comparator::no, var_name{"a"s}, ""s}, "kuku"s);
	parse_check_blocks( "<%if a%>kuku<%endif%>"sv, 0, result);
	parse_check_blocks( "<%if a %>kuku<%endif%>"sv, 0, result);
	result = tests::make_sblock(flags, st_if{comparator::no, var_name{"a"s,"is"s,"b"s}, ""s}, "kuku"s);
	parse_check_blocks( "<%if a.is.b%>kuku<%endif%>"sv, 0, result);
	parse_check_blocks( "<%if a.is.b %>kuku<%endif%>"sv, 0, result);
}
BOOST_AUTO_TEST_SUITE_END() // op_if
BOOST_AUTO_TEST_SUITE(op_for)
BOOST_DATA_TEST_CASE(
	  simple
	,   ut::data::make("kuku"s, ""s, "a"s, "aa"s, "abc"s)
	  * ut::data::make("for a in q"s, "for a in q "s, " for a in q "s)
	  * ut::data::make("endfor"s, "endfor "s, " endfor "s, "\tendfor\t"s)
	, text, start, finish )
{
	using cppjinja::s_block;
	using cppjinja::var_name;
	using st_for = cppjinja::st_for<std::string>;

	std::string data = "<%"s + start + "%>"s + text + "<%"s + finish + "%>"s;
	auto flags = tests::make_mbflags(text.empty(), true);
	auto result = tests::make_sblock(flags, st_for{{var_name{"a"s}}, var_name{"q"s}}, text);
	parse_check_blocks( data, 0, result);
}
BOOST_AUTO_TEST_SUITE_END() // op_for
BOOST_DATA_TEST_CASE(
	  raw
	,   ut::data::make("kuku"s, "kuku<<"s, "kuku<%if a%>"s, ""s, "a", "aa"s)
	  * ut::data::make("<% raw %>"s, "<% raw%>"s, "<%raw %>"s, "<%raw%>"s)
	  * ut::data::make("<% endraw %>"s, "<% endraw%>"s, "<%endraw %>"s, "<%endraw%>"s)
	, text, start, finish )
{
	using cppjinja::st_raw;
	using cppjinja::s_block;
	using cppjinja::parse;

	std::string data = start + text + finish;
	auto flags = tests::make_mbflags(text.empty(), true);
	auto result = tests::make_sblock(flags, st_raw{}, text);
	parse_check_blocks( data, 0, result );
}
BOOST_DATA_TEST_CASE( comment, ut::data::make(""s, "s"s, "aa"s, tests::random_string()), text)
{
	using cppjinja::s_block;
	using cppjinja::comparator;
	using cppjinja::var_name;
	using cppjinja::parse;
	using st_if = cppjinja::st_if<std::string>;
	using st_comment = cppjinja::st_comment<std::string>;

	auto flags = tests::make_mbflags(false,true);
	auto result = tests::make_sblock(flags, st_if{comparator::no, var_name{"a"s}, ""s}, st_comment{text});
	parse_check_blocks("<% if a%><#" + text + "#><%endif%>", 0, result);
}
BOOST_DATA_TEST_CASE(
	  named_block
	,   ut::data::make("a"s, "aa"s, tests::random_alnum())
	  * ut::data::make(""s, "a", "ss"s, ",!#", tests::random_string())
	  * ut::data::make(" "s, "\t"s, "   \t "s)
	  * ut::data::make("block "s, "\tblock\t"s, "block    "s )
	, name, content, finish, start )
{
	auto flags = tests::make_mbflags(content.empty(), true);
	auto result = tests::make_sblock(flags, name, content);
	std::string data = "<%"s + start + name + finish+ "%>"s + content + "<%endblock%>"s;
	parse_check_blocks(data, 0, result);
}
using macparam = cppjinja::macro_parameter<std::string>;
BOOST_DATA_TEST_CASE(
	  macro
	,   ut::data::make(std::make_pair("macro m"s,"m"s), std::make_pair(" macro mm"s,"mm"s), std::make_pair("macro\tasdf"s,"asdf"s))
	  * ut::data::make(""s, "cnt"s)
	  * ut::data::make(
		    std::make_pair(")"s, std::vector<macparam>{})
		  , std::make_pair("a  ) "s, std::vector<macparam>{macparam("a"s)})
		  , std::make_pair("a,b)\t"s, std::vector<macparam>{macparam("a"s),macparam("b"s)})
		  , std::make_pair("a,b='c')\t"s, std::vector<macparam>{macparam("a"s),macparam("b"s, "c"s)})
		  , std::make_pair( "a,b=c.d)\t"s, std::vector<macparam>{macparam("a"s),macparam("b"s, cppjinja::var_name{"c"s,"d"s})} )
		  )
	, name, content, params)
{
	using st_macro = cppjinja::st_macro<std::string>;

	auto flags = tests::make_mbflags(content.empty(), true);
	auto result = tests::make_sblock(flags, st_macro{name.second, params.second}, content);
	std::string data = "<%"s + name.first + "("s + params.first + "%>"s + content + "<%endmacro%>"s;
	parse_check_blocks(data, 0, result);
}
BOOST_AUTO_TEST_SUITE_END() // blocks
