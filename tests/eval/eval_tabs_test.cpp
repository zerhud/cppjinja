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

BOOST_AUTO_TEST_CASE(inner_content)
{
	mocks::data_provider prov;
	auto data = "<%block a %>\nok<% endblock %>"sv;
	BOOST_TEST( parse_single(data, prov) == "\nok"sv );
	data = "<%block a +%>\nok<% endblock %>"sv;
	BOOST_TEST( parse_single(data, prov) == "ok"sv );
	data = "<%block a %>ok\n<% endblock %>"sv;
	BOOST_TEST( parse_single(data, prov) == "ok\n"sv );
	data = "<%block a %>ok\n<%+ endblock %>"sv;
	BOOST_TEST( parse_single(data, prov) == "ok"sv );
}
