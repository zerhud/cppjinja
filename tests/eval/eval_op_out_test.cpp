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
#include "evtree/evtree.hpp"

#include "parser/grammar/tmpls.hpp"

#include "mocks.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;
namespace east = cppjinja::east;
namespace tdata = boost::unit_test::data;
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

std::string parse_single(
          std::string_view data
        , cppjinja::data_provider& prov
        , std::vector<std::string_view> extra={}
        )
{
	std::stringstream result;

	BOOST_TEST_CONTEXT("primary == " << data)
	{
		std::vector<ast::tmpl> tmpls;

		tmpls.emplace_back() = parse_tmpl(data);
		for(auto& ex:extra) tmpls.emplace_back() = parse_tmpl(ex);

		cppjinja::evtree ev;
		for(auto& t:tmpls) ev.add_tmpl(t);

		BOOST_CHECK_NO_THROW( ev.render(result, prov, "") );
	}

	return result.str();
}

struct mock_data_provider_fixture
{
	std::unique_ptr<mocks::data_provider> data =
	        std::make_unique<mocks::data_provider>();
};


BOOST_AUTO_TEST_SUITE(old_integrations_tests)

auto block_macro_data =
        tdata::make("block"s,    "macro"s)
      ^ tdata::make("endblock"s, "endmacro"s);
auto rvterm_data = tdata::make(
            "data"sv, "<= 'data' =>"sv, "<= 42 =>"sv, "<= ['one', 'two'] =>"sv
            ) ^ tdata::make(
            "data"sv, "data"sv,         "42"sv,       "[\"one\",\"two\"]"sv );
BOOST_DATA_TEST_CASE_F(mock_data_provider_fixture, render_vterm
         , rvterm_data , code, result)
{
	BOOST_TEST(parse_single(code, *data) == result);
}

BOOST_AUTO_TEST_SUITE(var_name)
	BOOST_FIXTURE_TEST_CASE(not_setted, mock_data_provider_fixture)
	{
		MOCK_EXPECT(data->value_var_name)
		        .once()
		        .with(east::var_name{"data"_s})
		        .returns("test"_ad)
		        ;
		BOOST_TEST(parse_single("<= data =>"sv, *data) == "test"sv );
	}

	BOOST_FIXTURE_TEST_CASE(setted, mock_data_provider_fixture)
	{
		auto pdata = "<% set d='test' %><= d =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "test"sv );
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
	MOCK_EXPECT(data->value_var_name)
	        .once()
	        .with(east::var_name{"a"_s})
	        .returns("not ok"_ad)
	        ;
	MOCK_EXPECT(data->value_function_call)
	        .once()
	        .calls([](const east::function_call& c){
				BOOST_TEST( c.ref.size() == 1 );
				BOOST_TEST( c.ref[0] == "filter"s );
				BOOST_TEST( c.params.size() == 1 );
				BOOST_TEST( *c.params.at(0).name == "$" );
				BOOST_TEST( *c.params.at(0).val == "not ok"s );
				return "ok"_ad;
			});

	auto pdata = "<= a | filter =>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "ok"sv );
}

BOOST_AUTO_TEST_CASE(function_from_provider)
{
	auto data = std::make_unique<mocks::data_provider>();
	MOCK_EXPECT(data->value_function_call)
	        .once()
	        .calls([](const east::function_call& c){
		BOOST_TEST( c.ref.size() == 1 );
		BOOST_TEST( c.ref[0] == "a"s );
		BOOST_TEST( c.params.size() == 1 );
		return "ok"_ad;
	});

	auto pdata = "<= a('p') =>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "ok"sv );
}

BOOST_AUTO_TEST_SUITE(access_to_variable_from_blocks)

	BOOST_FIXTURE_TEST_CASE(named_outputs, mock_data_provider_fixture)
	{
		auto pdata = "<% block bl %>ok<% endblock %>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "ok"sv );
		pdata = "<% block bl %>ok<% endblock %><= self.bl() =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "okok"sv );
		pdata = "<% block bl %>ok<% endblock %><= bl() =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "okok"sv );
	}

	BOOST_FIXTURE_TEST_CASE(no_macro_output, mock_data_provider_fixture)
	{
		auto pdata = "<% macro bl %>ok<% endmacro %>"sv;
		BOOST_TEST(parse_single(pdata, *data) == ""sv );
		pdata = "<% macro bl %>ok<% endmacro %><= bl() =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "ok"sv );
	}

	BOOST_FIXTURE_TEST_CASE(has_params_no_output, mock_data_provider_fixture)
	{
		auto pdata = "<% block bl(a) %>a<= a =><% endblock %>"sv;
		BOOST_TEST(parse_single(pdata, *data) == ""sv );
	}

	BOOST_DATA_TEST_CASE_F(mock_data_provider_fixture, with_params
	             , block_macro_data , open, close )
	{
		auto pdata =
		        "<% "s + open + " bl(a) %>a<= a =><% "s
		        + close + " %><= bl('ok') =>"s;
		BOOST_TEST(parse_single(pdata, *data) == "aok"sv );
	}

	BOOST_DATA_TEST_CASE_F(mock_data_provider_fixture, default_params,
	                       block_macro_data, open, close)
	{
		auto pdata =
		        "<% "s + open + " bl(a, b='bok') %><= b =><% "s + close + " %>"s
		        "<= bl('ok') =><= bl('ok', 'test') =>"s;
		BOOST_TEST(parse_single(pdata, *data) == "boktest"s );
	}

	BOOST_FIXTURE_TEST_CASE(macro_access_outuer_set, mock_data_provider_fixture)
	{
		auto pdata =
		        "<% set u='outer' %><% set a='bad' %><% macrobl(a) %>"
		        "<= a =><= u =><% endmacro %><=  bl('ok') =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "okouter"sv );
	}

	BOOST_FIXTURE_TEST_CASE(block_cannt_access_outuer_set, mock_data_provider_fixture)
	{
		MOCK_EXPECT(data->value_var_name).once().returns("out"_ad);
		auto pdata =
		        "<% set u='outer' %><% set a='bad' %><% block bl(a) %>"
		        "<= a =><= u =><% endblock %><=  bl('ok') =>"sv;
		BOOST_TEST(parse_single(pdata, *data) == "okout"sv );
	}
BOOST_AUTO_TEST_SUITE_END() // access_to_variable_from_blocks

BOOST_AUTO_TEST_SUITE(binary_ops)
BOOST_AUTO_TEST_CASE(simple_if)
{
	auto data = std::make_unique<mocks::data_provider>();
	auto pdata = "<% if 1 == 1 %>simple<% endif %>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "simple"sv );
	pdata = "<% if 1 == 1 %>simple<% endif %>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "simple"sv );
	pdata = "<% if 1 != 1 %>simple<% endif %>"sv;
	BOOST_TEST(parse_single(pdata, *data) == ""sv );
	pdata = "<% if 1 == 2 %>not ok<% else %>ok<% endif %>"sv;
	BOOST_TEST(parse_single(pdata, *data) == "ok"sv );
}
BOOST_AUTO_TEST_SUITE_END() // binary_ops

BOOST_AUTO_TEST_SUITE_END() // old_integrations_tests
