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
namespace aps = cppjinja::ast::expr_ops;

ast::op_include make_op_include(ast::string_t fn, std::optional<bool> im, std::optional<bool> wc)
{
	ast::op_include ret;
	ret.filename = std::move(fn);
	ret.ignore_missing = im;
	ret.with_context = wc;
	return ret;
}

BOOST_AUTO_TEST_SUITE(phase_parse)
BOOST_AUTO_TEST_SUITE(singles)

BOOST_DATA_TEST_CASE(op_out, utd::make("'a'"s, "foo()|e|a()|b('v')"s), data)
{
	ast::op_out result;

	std::string text = "<="s + data + "=>"s;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(txt::op_out, text) );
	BOOST_TEST( !result.open.trim );
	BOOST_TEST( !result.close.trim );

	text = "<=+"s + data + "+=>"s;
	BOOST_CHECK_NO_THROW( result = cppjinja::text::parse(txt::op_out, text) );
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
              aps::expr{aps::term{1}}
            , aps::single_var_name{ "a"s }
            , aps::term{"a"s}
            )
      , data, good_result)
{
	ast::op_set result;

	std::string text = "<% set x="s + data + "%>"s;
	BOOST_TEST_CONTEXT("TEXT = " << text)
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::op_set, text) );

	BOOST_TEST(result.value.names.size() == 1);
	BOOST_TEST(result.value.names.at(0) == aps::lvalue{aps::single_var_name{"x"s}});
	BOOST_TEST(result.value.value.get() == good_result);
}

BOOST_DATA_TEST_CASE(
        op_include
        , utd::make("'file'"s, "'file' ignore missing"s, "'f' with context"s)
        ^ utd::make(
              make_op_include("file", std::nullopt, std::nullopt)
            , make_op_include("file", true, std::nullopt)
            , make_op_include("f", std::nullopt, true)
            )
        , data, good_result)
{
	ast::op_include result;
	std::string text = "<% include "+data+"%>"s;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_include, text) );
	BOOST_TEST( result == good_result );
}

BOOST_AUTO_TEST_CASE(op_import)
{
	ast::op_import result;
	std::string text = "<% import 'file' as variable %>"s;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_import, text) );
	BOOST_TEST(result.as == "variable");
	BOOST_TEST(result.filename == "file");
	BOOST_TEST(result.tmpl_name == "");

	text = "<% import tmpl from 'file' as variable %>"s;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_import, text) );
	BOOST_TEST(result.as == "variable");
	BOOST_TEST(result.filename == "file");
	BOOST_TEST(result.tmpl_name == "tmpl");
}

BOOST_AUTO_TEST_SUITE_END() // singles
BOOST_AUTO_TEST_SUITE_END() // phase_parse
