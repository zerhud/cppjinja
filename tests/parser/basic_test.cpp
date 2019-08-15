/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cppjinja parser

#include <sstream>
#include <boost/test/unit_test.hpp>

#include "parser/grammar.hpp"

using namespace std::literals;
using cppjinja::parse;

BOOST_AUTO_TEST_CASE(just_str)
{
	auto block = cppjinja::parse("test string"sv);
	BOOST_REQUIRE_EQUAL(block.cnt.size(), 1);
	BOOST_REQUIRE(std::holds_alternative<std::string>(block.cnt[0]));
	BOOST_CHECK_EQUAL(std::get<std::string>(block.cnt[0]), "test string"s);
}
