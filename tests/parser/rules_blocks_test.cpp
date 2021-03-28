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
#include "parser/operators/expr.hpp"
#include "parser/operators/blocks.hpp"
#include "parser/grammar/expr.hpp"
#include "parser/grammar/blocks.hpp"
#include "parser/grammar/common.hpp"

using namespace std::literals;
namespace txt = cppjinja::text;
namespace ast = cppjinja::ast;
namespace est = cppjinja::ast::expr_ops;
namespace utd = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(phase_parse)

BOOST_AUTO_TEST_SUITE(block_raw)
BOOST_DATA_TEST_CASE(
	  good
    , utd::make("kuku"s, "'a'"s, "foo()"s, "<#"s,
                "<="s, "<%"s, "<%endraw_%>"s, "a b"s, "\nok"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, " "s, "\n"s)
	, data, left, right, space)
{
	ast::block_raw result;

	std::string text =
	        "<%"s + left + space + "raw"s + "%>"s +
	        data + "<%"s + space + "endraw" + right + "%>"s ;
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
		BOOST_TEST_REQUIRE( !result.else_block.has_value() );
		BOOST_TEST( result.condition == txt::parse(txt::expr_ops::expr_bool, condition) );
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
		BOOST_TEST_REQUIRE( !result.else_block.has_value() );
	}
}
BOOST_AUTO_TEST_CASE(else_thread)
{
	ast::block_if result;
	std::string text = "<% if 1==2 %>never seen<% else %>ok<% endif %>"s;
	BOOST_REQUIRE_NO_THROW( result = cppjinja::text::parse(txt::block_if, text) );
	BOOST_TEST_REQUIRE( result.else_block.has_value() );
	BOOST_TEST_REQUIRE( result.else_block->content.size() == 1 );
	BOOST_TEST( result.else_block->left_open.bsign.has_value() == false);
	BOOST_TEST( result.else_block->left_close.bsign.has_value() == false);

	text = "<% if 1==2 %> never seen <% else %> ok <% endif %>"s;
	BOOST_REQUIRE_NO_THROW( result = cppjinja::text::parse(txt::block_if, text) );
	BOOST_TEST_REQUIRE( result.content.size() == 1 );
	BOOST_TEST_REQUIRE( result.else_block.has_value() );
	BOOST_TEST_REQUIRE( result.else_block->content.size() == 1 );
	BOOST_TEST( result.else_block->left_open.bsign.has_value() == false);
	BOOST_TEST( result.else_block->left_close.bsign.has_value() == false);

	auto* else_cnt = boost::get<std::string>(&result.else_block->content[0]);
	BOOST_TEST_REQUIRE( else_cnt != nullptr );
	BOOST_TEST( *else_cnt == " ok "s );

	auto* if_cnt = boost::get<std::string>(&result.content[0]);
	BOOST_TEST_REQUIRE( if_cnt != nullptr );
	BOOST_TEST( *if_cnt == " never seen "s );
}
BOOST_AUTO_TEST_SUITE_END() // block_if

BOOST_AUTO_TEST_SUITE(inner)
BOOST_DATA_TEST_CASE(
	  content
	, utd::make("t"s, "<=a=>"s, "ttt<#c#>"s, "t<#cmt#><=a=>"s)
	^ utd::make(
		  ast::make_content( "t"s )
        , ast::make_content( ast::make_out("a"))
		, ast::make_content( "ttt"s, ast::make_comment("c"s))
        , ast::make_content( "t"s, ast::make_comment("cmt"s), ast::make_out("a"))
	  )
	, data, good_result)
{
	std::vector<ast::block_content> result;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_content_vec, data) );
	BOOST_TEST( result.size() == good_result.size() );
	BOOST_TEST( result == good_result );
}
BOOST_AUTO_TEST_SUITE_END() // inner

BOOST_DATA_TEST_CASE(
   block_for
  , utd::make("key, val in dict"s, "key in kuku"s, "key in kuku recursive"s)
  ^ utd::make(
        ast::make_for({"key"s, "val"s}, est::expr{est::single_var_name{"dict"s}})
      , ast::make_for({"key"s}, est::expr{est::single_var_name{"kuku"s}})
      , ast::make_for({"key"s}, est::expr{est::single_var_name{"kuku"s}}, true)
 )
 , data, good_result)
{
	ast::block_for result;
	std::string text = "<% for "s + data + " %><%endfor%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text) {
		BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_for, text) );
		BOOST_TEST( result.content.size() == good_result.content.size() );
		BOOST_TEST( result.content == good_result.content );
		BOOST_TEST( result == good_result );
		BOOST_CHECK( !result.else_block.has_value() );
	}
}

BOOST_AUTO_TEST_CASE(block_for_else)
{
	ast::block_for result;
	std::string text = "<% for key in keys %>f<% else %>e<%endfor%>"s;
	result = txt::parse(txt::block_for, text);
	BOOST_CHECK( result.else_block.has_value() );
	BOOST_TEST( result.content.size() == 1);
	BOOST_TEST( result.else_block->content.size() == 1);
}

BOOST_DATA_TEST_CASE(
	  block_macro
	, utd::make("macro m"s, "macro m()"s, "macro m(n, n=v)"s)
	^ utd::make(
		  ast::make_macro("m"s, {})
		, ast::make_macro("m"s, {})
		, ast::make_macro("m"s, {ast::macro_parameter{"n",{}}, ast::macro_parameter{"n", est::expr{est::single_var_name{"v"}}}})
		)
	, data, good_result)
{
	ast::block_macro result;
	std::string text = "<%"s + data + "%><%endmacro%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text) {
		BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_macro, text) );
		BOOST_TEST( result == good_result );
	}

	text = "<%"s + data + "%><%endmacro m%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text)
		BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_macro, text) );
}

BOOST_AUTO_TEST_CASE(block_set)
{
	std::string data = "<% set name|e %>content<%endset%>"s;
	ast::block_set good_result;
	good_result.name = "name"s;
	good_result.filters.emplace_back(
	            ast::expr_ops::filter_call{
	                est::lvalue{est::single_var_name{"e"s}}, {}});
	good_result.content.emplace_back("content"s);

	ast::block_set result;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::block_set, data) );
	BOOST_TEST( result == good_result );
}

BOOST_AUTO_TEST_CASE(block_call)
{
	std::string data = "<% call(kuku) lala(name=1) %>content<%endcall%>"s;
	ast::block_call result ;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::block_call, data) );
	BOOST_TEST(result.name == "lala"s);
	BOOST_TEST(result.params.size() == 1);
	BOOST_TEST(result.call_params.size() == 1);
	data = "<% call name %>content<%endcall%>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::block_call, data) );
	BOOST_TEST(result.name == "name"s);
	BOOST_TEST(result.params.size() == 0);
	BOOST_TEST(result.call_params.size() == 0);
}

BOOST_AUTO_TEST_CASE(filtered)
{
	std::string data = "<% filter name %>cnt<%endfilter%>"s;
	ast::block_filtered result;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::block_filtered, data) );
}

BOOST_AUTO_TEST_SUITE_END() // phase_parse
