/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE grammar

#include <bitset>
#include <sstream>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/operators/common.hpp"
#include "parser/grammar_def/common.hpp"

using namespace std::literals;

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;
namespace ast = cppjinja::ast;

BOOST_DATA_TEST_CASE(
        quoted1_string
      ,   utd::make(u8"'kuku'"s, u8"''"s, u8"'привет'"s, u8"'k\\k'"s, u8"'ok\\'ok'"s)
        ^ utd::make(u8"kuku"s,   u8""s,   u8"привет"s  , u8"k\\k"s,   u8"ok'ok"s)
      , data, good_result
      )
{
	std::string result;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(cppjinja::text::quoted_string, data) );
	BOOST_TEST( result == good_result );
}

BOOST_DATA_TEST_CASE(
        single_var_name
      ,   utd::make("kuku"s, ""s, "a8_"s, "8a"s)
        ^ utd::make("kuku"s, ""s, "a8_"s, ""s)
      , data, good_result
      )
{
	std::string result;

	if(good_result.empty())
		BOOST_CHECK_THROW( result = cppjinja::text::parse(cppjinja::text::single_var_name, data), std::exception );
	else
		BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(cppjinja::text::single_var_name, data) );

	BOOST_TEST( result == good_result );
}

BOOST_DATA_TEST_CASE(
          var_name
        ,   utd::make("a"s, "a.b"s)
          ^ utd::make(ast::var_name{"a"s}, ast::var_name{"a"s, "b"s})
        , data, good_result)
{
	ast::var_name result;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(cppjinja::text::var_name, data) );
	BOOST_TEST( result == good_result );
}

BOOST_DATA_TEST_CASE(
          value_term
        , utd::make("'a'"s, "a"s, "a.a"s)
        ^ utd::make(
              ast::value_term{"a"s}
            , ast::var_name{"a"s}
            , ast::var_name{"a"s, "a"s}
            )
        , data, good_result)
{
	ast::value_term result;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(cppjinja::text::value_term, data) );
	BOOST_TEST( result == good_result );
}
