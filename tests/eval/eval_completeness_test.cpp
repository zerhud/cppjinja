/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator completeness

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
using namespace std::literals;

namespace data = boost::unit_test::data;

std::string parse_single(
          std::string_view data
        , cppjinja::data_provider& prov
        )
{
	std::stringstream result;

	BOOST_TEST_CONTEXT("primary == " << data)
	{
		cppjinja::evtree ev;
		auto tmpl = txt::parse(txt::tmpl, data);
		ev.add_tmpl( tmpl );

		BOOST_CHECK_NO_THROW( ev.render(result, prov, "") );
	}

	return result.str();
}

BOOST_AUTO_TEST_SUITE(extra_integrated_checks)
BOOST_DATA_TEST_CASE( simple_string
      , data::make("ok"sv, " \nok"sv)
      , data )
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == "ok"sv );
}

BOOST_DATA_TEST_CASE( blocks
      , data::make("ok<% block bl %>ok<% endblock %>"sv,
                   "ok<% block bl %>o<% block i%>k<% endblock %><% endblock %>"sv)
      , data)
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == "okok"sv );
}

BOOST_DATA_TEST_CASE(set_var
      , data::make("<% set a='okok' %><= a =>"sv
                  ,"<% block bl %><% set a='okok' %><= a =><% endblock %>"sv
                  ,"<% set a='okok' %><% macro bl %><= a =><% endmacro %><= bl() =>"sv
                  )
      , data )
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == "okok"sv );
}

BOOST_AUTO_TEST_CASE(get_var_nesteed)
{
	namespace east = cppjinja::east;
	mocks::data_provider prov;
	MOCK_EXPECT(prov.value_var_name).once().calls([](east::var_name v){
		BOOST_TEST_REQUIRE(v.size()==1);
		BOOST_TEST(v[0]=="b");
		return east::value_term("b"s);
	});
	MOCK_EXPECT(prov.value_var_name).once().calls([](east::var_name v){
		BOOST_TEST_REQUIRE(v.size()==2);
		BOOST_TEST(v[0]=="a");
		BOOST_TEST(v[1]=="b");
		return east::value_term("ok"s);
	});
	BOOST_TEST( parse_single("<= a[b] =>", prov)=="ok"s );
}
BOOST_AUTO_TEST_SUITE_END() // extra_integrated_checks
