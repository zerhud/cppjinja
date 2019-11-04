/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE grammar

#include <sstream>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/operators/opterm.hpp"
#include "parser/grammar/opterm.hpp"

using namespace std::literals;
namespace txt = cppjinja::text;
namespace ast = cppjinja::ast;

BOOST_AUTO_TEST_CASE(start)
{
	std::string data = "<%";
	ast::op_term_start result;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_term_start, data) );
	BOOST_FAIL("empty test");
}
