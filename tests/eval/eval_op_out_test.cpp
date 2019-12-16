/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator op_out

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "evaluator/eval.hpp"

#include "parser/grammar/tmpls.hpp"

#include "mocks.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;
namespace east = cppjinja::east;
using namespace std::literals;

ast::tmpl parse_tmpl(std::string_view data)
{
	ast::tmpl tmpl;
	BOOST_TEST_CONTEXT("data    == " << data)
	{
		BOOST_REQUIRE_NO_THROW( tmpl = txt::parse(txt::tmpl, data) );
	}
	return tmpl;
}

std::string parse_single(std::string_view data, cppjinja::data_provider& prov, std::vector<std::string_view> extra={})
{
	std::stringstream result;

	BOOST_TEST_CONTEXT("primary == " << data)
	{
		std::vector<ast::tmpl> tmpls;

		tmpls.emplace_back() = parse_tmpl(data);
		for(auto& ex:extra) tmpls.emplace_back() = parse_tmpl(ex);

		cppjinja::evaluator ev(tmpls);

		BOOST_CHECK_NO_THROW( ev.render(result, prov) );
	}

	return result.str();
}

BOOST_AUTO_TEST_CASE(string)
{
	auto data = std::make_unique<mocks::data_provider>();
	BOOST_TEST(parse_single("<= 'data' =>"sv, *data) == "data"sv );
}

BOOST_AUTO_TEST_CASE(number)
{
	auto data = std::make_unique<mocks::data_provider>();
	BOOST_TEST(parse_single("<= 42 =>"sv, *data) == std::to_string(42.0) );
}

BOOST_AUTO_TEST_CASE(no_output)
{
	auto data = std::make_unique<mocks::data_provider>();
	BOOST_TEST( parse_single("<= ['one', 'two'] =>"sv, *data) == ""sv );
}

BOOST_AUTO_TEST_SUITE(var_name)

	BOOST_AUTO_TEST_CASE(not_setted)
	{
		auto data = std::make_unique<mocks::data_provider>();
		MOCK_EXPECT(data->render_var_name).once().with(ast::var_name{"data"s}).returns("test");
		BOOST_TEST(parse_single("<= data =>"sv, *data) == "test"sv );
	}

	BOOST_AUTO_TEST_CASE(setted)
	{
		auto data = std::make_unique<mocks::data_provider>();
		auto pdata = "<% set d='test' %><= d =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "test"sv );
	}

	BOOST_AUTO_TEST_CASE(only_single_name_search_allowed)
	{
		auto data = std::make_unique<mocks::data_provider>();
		MOCK_EXPECT(data->render_var_name).once().with(ast::var_name{"a"s,"d"s}).returns("up");
		auto pdata = "<% set d='test' %><= a.d =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "up"sv );
	}

BOOST_AUTO_TEST_SUITE_END() //var_name

BOOST_AUTO_TEST_CASE(block_raw)
{
	auto data = std::make_unique<mocks::data_provider>();
	auto pdata = "<% raw %><= a.d =><% endraw %>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "<= a.d =>"sv );
}

BOOST_AUTO_TEST_CASE(filter)
{
	ast::function_call call;
	call.ref = ast::var_name{"filter"s};

	auto data = std::make_unique<mocks::data_provider>();
	MOCK_EXPECT(data->render_var_name).once().with(ast::var_name{"a"s}).returns("not ok");
	MOCK_EXPECT(data->render_filter_call).once().calls([](const east::function_call& c, const std::string& b){
				BOOST_TEST( c.ref.size() == 1 );
				BOOST_TEST( c.ref[0] == "filter"s );
				BOOST_TEST( c.params.empty() );
				BOOST_TEST( b == "not ok"s );
				return "ok"s;
			});

	auto pdata = "<= a | filter =>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "ok"sv );
}
