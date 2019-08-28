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

#include "parser/grammar.hpp"

using namespace std::literals;
using cppjinja::parse;

template<typename Obj, typename Val>
void check_block(const Obj& obj, std::size_t ind, const Val& val)
{
	BOOST_REQUIRE_LE(ind+1, obj.cnt.size());
	BOOST_REQUIRE(std::holds_alternative<Val>(obj.cnt[ind]));
	BOOST_CHECK_EQUAL(std::get<Val>(obj.cnt[ind]), val);
}

template<typename Obj, typename Val, typename... Vals>
void check_block(const Obj& obj, std::size_t ind, const Val& val, const Vals&... vals)
{
	check_block(obj, ind, val);
	check_block(obj, ind+1, vals...);
}

BOOST_AUTO_TEST_CASE(just_str)
{
	check_block(cppjinja::parse("test string"sv), 0, "test string"s);
	check_block(cppjinja::parse("привет 你好，世界！"sv), 0, "привет 你好，世界！"s);
	check_block(cppjinja::parse(L"test string"sv), 0, "test string"s);
	check_block(cppjinja::parse(L"привет"sv), 0, "привет"s);
}

BOOST_AUTO_TEST_SUITE(output)
BOOST_AUTO_TEST_CASE(string)
{
	using cppjinja::parse;
	check_block(parse("test <= 'kuku' =>"), 0, "test "s, cppjinja::st_out<std::string>{ "kuku"s, {} });
	check_block(parse("test <= \"kuku\" =>"), 0, "test "s, cppjinja::st_out<std::string>{ "kuku"s, {} });
	check_block(parse("<= 'kuk\\'u' =>"), 0, cppjinja::st_out<std::string>{ "kuk'u"s, {} });
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
	using cppjinja::fnc_call;
	using sto_t = cppjinja::st_out<std::string>;

	check_block(parse("<= foo() =>"sv), 0, sto_t{ fnc_call{"foo"s, {}}, {} });
	check_block(parse("<= foo(qq) =>"sv), 0, sto_t{ fnc_call{"foo"s, {var_name{"qq"s}}}, {} });
	check_block(parse("<= foo(q.q) =>"sv), 0, sto_t{ fnc_call{"foo"s, {var_name{"q"s, "q"s}}}, {} });
	check_block(parse("<= foo(qq, q.q) =>"sv), 0, sto_t{ fnc_call{"foo"s, {var_name{"qq"s}, var_name{"q"s, "q"s}}}, {} });
	check_block(parse("<= foo('qq') =>"sv), 0, sto_t{ fnc_call{"foo"s, {"qq"s}}, {} });
}
BOOST_AUTO_TEST_SUITE_END() // output
