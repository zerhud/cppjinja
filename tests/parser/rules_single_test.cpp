/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_single

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/operators/single.hpp"
#include "parser/grammar/single.hpp"

using namespace std::literals;
namespace txt = cppjinja::text;
namespace ast = cppjinja::ast;
namespace utd = boost::unit_test::data;

BOOST_DATA_TEST_CASE(
	  op_out
	, utd::make("kuku"s, "'a'"s, "foo()"s)
	^ utd::make(
		  ast::value_term{ast::var_name{"kuku"}}
		, "a"s
		, ast::function_call{ast::var_name{"foo"}, {}}
		)
	, data, good_result)
{
	ast::op_out result;

	std::string text = "<="s + data + "=>"s;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(txt::op_out, text) );
	BOOST_TEST( result.value == good_result );
	BOOST_TEST( !result.open.trim );
	BOOST_TEST( !result.close.trim );

	text = "<=+"s + data + "+=>"s;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(txt::op_out, text) );
	BOOST_TEST( result.value == good_result );
	BOOST_TEST( result.open.trim );
	BOOST_TEST( result.close.trim );
}

BOOST_DATA_TEST_CASE(
	  op_comment
	, utd::make("kuku"s, ""s, "<% <= <#"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, "+"s)
	, data, left, right)
{
	ast::op_comment result;

	std::string text = "<#"s + left + data + right + "#>"s;

	if(data.empty())
		BOOST_REQUIRE_THROW( result = cppjinja::text::parse(txt::op_comment, text), std::exception );
	else BOOST_REQUIRE_NO_THROW( result = cppjinja::text::parse(txt::op_comment, text) );

	BOOST_TEST( result.value == data );
	if(!data.empty()) {
		BOOST_TEST( !left.empty() == result.open.trim );
		BOOST_TEST( !right.empty() == result.close.trim );
	}
}

BOOST_DATA_TEST_CASE(
        op_set
      , utd::make("1"s, "a"s, "'a'"s)
      ^ utd::make(
              ast::value_term{1}
            , ast::var_name{ "a"s }
            , "a"s
            )
      , data, good_result)
{
	ast::op_set result;

	std::string text = "<% set x="s + data + "%>"s;
	BOOST_TEST_CONTEXT("TEXT = " << text)
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::op_set, text) );

	BOOST_TEST(result.name == ast::var_name{"x"s});
	BOOST_TEST(result.value == good_result);
}

