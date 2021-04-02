/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE parser errors

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "parser/grammar/tmpls.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;

BOOST_AUTO_TEST_SUITE(phase_parse)
BOOST_AUTO_TEST_SUITE(errors)
using cppjinja_mocks::parser_fixture;
BOOST_FIXTURE_TEST_CASE(tmpl, parser_fixture)
{
	expect_fn("test");
	MOCK_EXPECT(env->on_error);
	txt::parse(txt::file, "<% template tmpl %> <%endtemplate%>", env.get());
	BOOST_TEST(eout.str().find("In file test") != std::string::npos);
	BOOST_TEST(eout.str().find("block_content_vec") != std::string::npos);
}
BOOST_AUTO_TEST_SUITE_END() // errors
BOOST_AUTO_TEST_SUITE_END() // phase_parse
