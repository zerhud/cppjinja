/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE loader

#include <sstream>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "loader/parser.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(loader)

BOOST_AUTO_TEST_CASE(simple)
{
	cppjinja::parser psr({});
	std::stringstream data("some tmpl"s);
	BOOST_CHECK_NO_THROW( psr.parse(data) );
	auto tmpls = psr.tmpls();
	auto files = psr.files();

	BOOST_TEST_REQUIRE( files.size() == 1 );
	BOOST_TEST( files[0].tmpls.size() == 1 );

	BOOST_TEST_REQUIRE( tmpls.size() == 1 );
	BOOST_TEST( tmpls[0].name.empty() );
	BOOST_TEST_REQUIRE( tmpls[0].content.size() == 1 );
}

BOOST_AUTO_TEST_SUITE_END() // loader
