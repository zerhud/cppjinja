/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE parser helpers

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/helpers.hpp"

BOOST_AUTO_TEST_SUITE(phase_parse)
BOOST_AUTO_TEST_SUITE(helpers)

BOOST_AUTO_TEST_CASE(has_str_name)
{
	using namespace cppjinja;
	struct hn{ std::string name; };
	struct hwn{ int name; };
	struct noname{ int kuku; };

	static_assert ( has_str_name_v<hn>, "struct has string name" );
	static_assert (!has_str_name_v<hwn>, "struct has int name" );
	static_assert (!has_str_name_v<noname>, "struct has no name" );

	BOOST_TEST(  has_str_name_v<hn> );
	BOOST_TEST( !has_str_name_v<hwn> );
	BOOST_TEST( !has_str_name_v<noname> );
}

BOOST_AUTO_TEST_SUITE_END() // phase_parse
BOOST_AUTO_TEST_SUITE_END() // helpers
