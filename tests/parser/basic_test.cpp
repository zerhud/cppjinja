/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cppjinja parser

#include <iostream>
#include <sstream>
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

template<typename Ref, typename... Cnt>
auto make_rblock(Ref&& ref, Cnt&&... cnt)
{
	using cppjinja::s_block;
	using cnt_t = cppjinja::block_content<std::string>;
	return boost::recursive_wrapper( s_block{std::move(ref), cnt_t{std::move(cnt)...}} );
}

} // namespace tests {

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

template<typename... Vals>
void parse_check_block(std::string_view data, std::size_t ind, const Vals&... vals)
{
	BOOST_TEST_CONTEXT("Data was " << data) {
		cppjinja::s_block bl;
		BOOST_CHECK_NO_THROW(bl=cppjinja::parse(data));
		check_block(bl, ind, vals...);
	}
}

BOOST_AUTO_TEST_CASE(just_str)
{
	check_block(cppjinja::parse("test string"sv), 0, "test string"s);
	check_block(cppjinja::parse("привет 你好，世界！"sv), 0, "привет 你好，世界！"s);
	check_block(cppjinja::parse(L"test string"sv), 0, "test string"s);
	check_block(cppjinja::parse(L"привет"sv), 0, "привет"s);
	BOOST_TEST_CONTEXT("wide string, line " << __LINE__) {
		check_block(cppjinja::wparse(L"привет"sv), 0, L"привет"s);
	}
}

BOOST_AUTO_TEST_SUITE(output)
BOOST_AUTO_TEST_CASE(string)
{
	using cppjinja::parse;
	using cppjinja::wparse;
	parse_check_block("test <= 'kuku' =>"sv, 0, "test "s, cppjinja::st_out<std::string>{ "kuku"s, {} });
	parse_check_block("test <= \"kuku\" =>"sv, 0, "test "s, cppjinja::st_out<std::string>{ "kuku"s, {} });
	parse_check_block("<= 'kuk\\'u' =>"sv, 0, cppjinja::st_out<std::string>{ "kuk'u"s, {} });

	BOOST_TEST_CONTEXT("Russian data, line " << __LINE__ ) {
		check_block(wparse(L"<= 'привет' =>"), 0, cppjinja::st_out<std::wstring>{ L"привет"s, {} });
		check_block(wparse(L"п<= 'привет' =>"), 0, L"п"s, cppjinja::st_out<std::wstring>{ L"привет"s, {} });
	}
}
BOOST_AUTO_TEST_CASE(variable)
{
	using cppjinja::parse;
	using sto_t = cppjinja::st_out<std::string>;

	cppjinja::s_block block_glob = parse("<= var =>"sv);
	cppjinja::s_block block_addr = parse("<= foo.bar =>"sv);

	check_block(block_glob, 0, sto_t{ cppjinja::var_name{"var"}, {} } );
	check_block(block_addr, 0, sto_t{ cppjinja::var_name{"foo", "bar"}, {} } );
}
BOOST_AUTO_TEST_CASE(function)
{
	using cppjinja::parse;
	using cppjinja::var_name;
	using sto_t = cppjinja::st_out<std::string>;
	using fnc_call = cppjinja::fnc_call<std::string>;

	parse_check_block("q<= foo() =>"sv, 0, "q"s, sto_t{ fnc_call{{"foo"s}, {}}, {} });
	parse_check_block("<= foo(qq) =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"qq"s}}}, {} });
	parse_check_block("<= foo(q.q) =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"q"s, "q"s}}}, {} });
	parse_check_block("<= foo(qq, q.q) =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"qq"s}, var_name{"q"s, "q"s}}}, {} });
	parse_check_block("<= foo('qq') =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {"qq"s}}, {} });
}
BOOST_AUTO_TEST_CASE(filters)
{
	using cppjinja::parse;
	using cppjinja::var_name;
	using sto_t = cppjinja::st_out<std::string>;
	using fnc_call = cppjinja::fnc_call<std::string>;

	parse_check_block("<= foo() | filter =>"sv, 0, sto_t{ fnc_call{{"foo"s}, {}}, {var_name{"filter"s}} });
	parse_check_block("<= foo(qq)|f1|f2=>"sv, 0, sto_t{ fnc_call{{"foo"s}, {var_name{"qq"s}}}, {var_name{"f1"s},var_name{"f2"s}} });
	parse_check_block("text<= 'q'|f =>"sv, 0, "text"s, sto_t{ "q"s, {var_name{"f"s}} });
	parse_check_block("<= 'q'|f() =>"sv, 0, sto_t{ "q"s, {fnc_call{{"f"s},{}}} });
	parse_check_block("<= 'q'|f('q') =>"sv, 0, sto_t{ "q"s, {fnc_call{{"f"s},{"q"s}}} });
	parse_check_block("<= 'q'|a.f('q',q) =>"sv, 0, sto_t{ "q"s, {fnc_call{{"a"s,"f"s},{"q"s,var_name{"q"s}}}} });
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
	auto result = tests::make_rblock(st_if{comparator::eq, var_name{"a"s}, var_name{"b"s}}, text);
	if(text.empty()) result.get().cnt.clear();

	parse_check_block(data, 0, result);
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

	auto result = tests::make_rblock(st_if{comparator::no, var_name{"a"s}, ""s}, "kuku"s);
	parse_check_block( "<%if a%>kuku<%endif%>"sv, 0, result);
	parse_check_block( "<%if a %>kuku<%endif%>"sv, 0, result);
	result = tests::make_rblock(st_if{comparator::no, var_name{"a"s,"is"s,"b"s}, ""s}, "kuku"s);
	parse_check_block( "<%if a.is.b%>kuku<%endif%>"sv, 0, result);
	parse_check_block( "<%if a.is.b %>kuku<%endif%>"sv, 0, result);
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
	auto result = tests::make_rblock(st_for{{var_name{"a"s}}, var_name{"q"s}}, text);
	if(text.empty()) result.get().cnt.clear();
	parse_check_block( data, 0, result);
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
	BOOST_TEST_CONTEXT("Data was" << data) {
		s_block bl;
		BOOST_REQUIRE_NO_THROW(bl = parse(data));
		auto& rb = std::get<boost::recursive_wrapper<s_block>>(bl.cnt[0]);
		if(text.empty())  BOOST_CHECK(rb.get().cnt.empty());
		else {
			BOOST_REQUIRE(std::holds_alternative<std::string>(rb.get().cnt[0]));
			BOOST_CHECK_EQUAL(std::get<std::string>(rb.get().cnt[0]), text);
		}
	}
}
BOOST_DATA_TEST_CASE( comment, ut::data::make(""s, "s"s, "aa"s, tests::random_string()), text)
{
	using cppjinja::s_block;
	using cppjinja::comparator;
	using cppjinja::var_name;
	using cppjinja::parse;
	using st_if = cppjinja::st_if<std::string>;
	using st_comment = cppjinja::st_comment<std::string>;

	auto result = tests::make_rblock(st_if{comparator::no, var_name{"a"s}, ""s}, st_comment{text});
	parse_check_block("<% if a%><#" + text + "#><%endif%>", 0, result);
}
BOOST_DATA_TEST_CASE(
	  named_block
	,   ut::data::make("a"s, "aa"s, tests::random_alnum())
	  * ut::data::make(""s, "a", "ss"s, ",!#", tests::random_string())
	  * ut::data::make(" "s, "\t"s, "   \t "s)
	  * ut::data::make("block "s, "\tblock\t"s, "block    "s )
	, name, content, finish, start )
{
	auto result = tests::make_rblock(name, content);
	if(content.empty()) result.get().cnt.clear();
	std::string data = "<%"s + start + name + finish+ "%>"s + content + "<%endblock%>"s;
	parse_check_block(data, 0, result);
}
BOOST_AUTO_TEST_SUITE_END() // blocks
