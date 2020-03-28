/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator tabs

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

BOOST_DATA_TEST_CASE(trim_left,
                     data::make({
         "<%block a +%>\nok<% endblock %>"sv,
         "<%block a +%> ok<% endblock %>"sv,
         "<%block a +%>\n ok<% endblock %>"sv,
                                }),
                     data)
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == "ok"sv );
}

BOOST_DATA_TEST_CASE(trim_right,
                     data::make({
         "<%block a %>ok\n<%+ endblock %>"sv,
         "<%block a %>ok <%+ endblock %>"sv,
         "<%block a %>ok\n  <%+ endblock %>"sv,
                                }),
                     data)
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == "ok"sv );
}

BOOST_DATA_TEST_CASE(trim_both,
                     data::make({
         "<%block a +%> ok\n<%+ endblock %>"sv,
         "<%block a +%>\n ok <%+ endblock %>"sv,
         "<%block a +%> ok\n  <%+ endblock %>"sv,
         "<%block a %><% if 1 == 1 +%> ok <%+ endif %><% endblock %>"sv,
                                }),
                     data)
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == "ok"sv );
}

BOOST_DATA_TEST_CASE(no_trim,
                     data::make({
         "<%block a %> ok <% endblock %>"sv,
         "<%block a %><% if 1 == 1 %> ok <% endif %><% endblock %>"sv,
         "<%block a %><% if 1 == 1 %> ok <% endif %><%+ endblock %>"sv,
                                }),
                     data)
{
	mocks::data_provider prov;
	BOOST_TEST( parse_single(data, prov) == " ok "sv );
}
