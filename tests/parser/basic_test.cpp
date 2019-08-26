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
	cppjinja::s_block block = cppjinja::parse("test <= kuku =>"sv);
	check_block(block, 0, "test "s);
	BOOST_REQUIRE_EQUAL(block.cnt.size(), 2);
	BOOST_CHECK(std::holds_alternative<cppjinja::st_out<std::string>>(block.cnt[1]));
	auto& op_out = std::get<cppjinja::st_out<std::string>>(block.cnt[1]);
	BOOST_REQUIRE(std::holds_alternative<std::string>(op_out.ref));
	BOOST_CHECK_EQUAL(std::get<std::string>(op_out.ref), "kuku"s);
}
BOOST_AUTO_TEST_SUITE_END() // output
