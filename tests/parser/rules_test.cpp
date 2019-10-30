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

#include "parser/grammar_def/common.hpp"
#include "parser/parse.hpp"

using namespace std::literals;

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;

BOOST_DATA_TEST_CASE(
        simple
      ,   utd::make(u8"'kuku'"s, u8"''"s, u8"'привет'"s)
        ^ utd::make(u8"kuku"s, u8""s, u8"привет"s)
      , data, good_result
      )
{
	std::string result;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(cppjinja::text::quoted_string, data) );
	BOOST_TEST( result == good_result );
}
