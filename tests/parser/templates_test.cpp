/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cppjinja parser

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/grammar.hpp"
#include "parser/debug_operators.hpp"

using namespace std::literals;

using sjtmpl = cppjinja::jtmpl<std::string>;
using wjtmpl = cppjinja::jtmpl<std::wstring>;

BOOST_AUTO_TEST_CASE(simple)
{
	auto data = "<%template tmpl%>A<%block b%>B<%endblock%><%endtemplate%>"sv;
	std::vector<sjtmpl> ast;
	BOOST_CHECK_NO_THROW( ast = cppjinja::parse(data));
	BOOST_TEST( ast.size() == 1 );
	BOOST_TEST_REQUIRE( 0 < ast.size(), "template are not parsed" );
}

