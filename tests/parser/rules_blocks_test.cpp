/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_blocks

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include "parser/parse.hpp"
#include "parser/operators/blocks.hpp"
#include "parser/grammar/blocks.hpp"
#include "parser/grammar/common.hpp"

using namespace std::literals;
namespace txt = cppjinja::text;
namespace ast = cppjinja::ast;
namespace utd = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(block_raw)
BOOST_DATA_TEST_CASE(
	  good
	, utd::make("kuku"s, "'a'"s, "foo()"s, "<#"s, "<="s, "<%"s, "<%endraw_%>"s, "a b"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, " "s, "\n"s)
	, data, left, right, space)
{
	ast::block_raw result;

	std::string text = "<%"s + left + space + "raw"s + "%>"s + data + "<%"s + space + "endraw" + right + "%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text) {
		BOOST_REQUIRE_NO_THROW( result = cppjinja::text::parse(txt::block_raw, text) );
		BOOST_TEST( result.value == data );
		BOOST_TEST( !left.empty() == result.left_open.trim );
		BOOST_TEST( !right.empty() == result.right_close.trim );
	}
}
BOOST_AUTO_TEST_SUITE_END() // block_raw

BOOST_AUTO_TEST_SUITE(block_if)
BOOST_DATA_TEST_CASE(
	  good
	, utd::make("kuku"s, "'a'"s, "foo()"s, "a b"s, ""s)
	* utd::make("a==b"s, "a in b"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, " "s, "\n"s)
	, data, condition, left, right, space)
{
	ast::block_if result;

	std::string text = "<%"s + left + space + "if "s + condition + "%>"s + data + "<%"s + space + "endif" + right + "%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text) {
		BOOST_REQUIRE_NO_THROW( result = cppjinja::text::parse(txt::block_if, text) );
		BOOST_TEST( result.condition == txt::parse(txt::binary_op, condition) );
		BOOST_TEST( !left.empty() == result.left_open.trim );
		BOOST_TEST( !right.empty() == result.right_close.trim );
		if(!data.empty()) {
			BOOST_REQUIRE_EQUAL( result.content.size(), 1 );
			BOOST_TEST( boost::get<ast::string_t>(result.content[0])==data );
		}
	}
}
BOOST_DATA_TEST_CASE(
	  bad
	, utd::make("a<#"s, "<#"s, "<="s, "<%"s, "<% raw %>"s)
	* utd::make("a==b"s, "a in b"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, " "s, "\n"s)
	, data, condition, left, right, space)
{
	ast::block_if result;

	std::string text = "<%"s + left + space + "if "s + condition + "%>"s + data + "<%"s + space + "endif" + right + "%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text) {
		BOOST_REQUIRE_NO_THROW( txt::parse(txt::binary_op, condition) );
		BOOST_REQUIRE_THROW( result = cppjinja::text::parse(txt::block_if, text), std::exception );
	}
}
BOOST_AUTO_TEST_SUITE_END() // block_if

BOOST_AUTO_TEST_SUITE(inner)
BOOST_DATA_TEST_CASE(
	  content
	, utd::make("t"s, "<=a=>"s, "ttt<#c#>"s, "t<#cmt#><=a=>"s)
	^ utd::make(
		  ast::make_content( "t"s )
		, ast::make_content( ast::make_out(ast::var_name{"a"s}))
		, ast::make_content( "ttt"s, ast::make_comment("c"s))
		, ast::make_content( "t"s, ast::make_comment("cmt"s), ast::make_out(ast::var_name{"a"s}))
	  )
	, data, good_result)
{
	std::vector<ast::block_content> result;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_content_vec, data) );
	BOOST_TEST( result.size() == good_result.size() );
	BOOST_TEST( result == good_result );
}
BOOST_AUTO_TEST_SUITE_END() // inner

