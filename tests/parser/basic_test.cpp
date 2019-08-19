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
	//BOOST_REQUIRE_LQ(obj.cnt.size(), ind);
	BOOST_REQUIRE(std::holds_alternative<Val>(obj.cnt[ind]));
	BOOST_CHECK_EQUAL(std::get<Val>(obj.cnt[ind]), val);
}

template<typename Val, typename... Vals>
void check_block(const Obj& obj, std::size_t ind, const Val& val, const Vals&... vals)
{
	check_block(obj, ind, val);
	check_block(obj, ind+1, vals...);
}

BOOST_AUTO_TEST_CASE(just_str)
{
	check_block(cppjinja::parse("test string"sv), 0, "test_string"s);
	check_block(cppjinja::parse(L"привет"sv), 0, L"привет"s);
}
