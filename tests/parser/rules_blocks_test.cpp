/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_blocks

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/operators/blocks.hpp"
#include "parser/grammar/blocks.hpp"

using namespace std::literals;
namespace txt = cppjinja::text;
namespace ast = cppjinja::ast;
namespace utd = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(block_raw)
BOOST_DATA_TEST_CASE(
	  good
	, utd::make("kuku"s, "'a'"s, "foo()"s, "<#"s, "<="s, "<%"s, "<%endraw_%>"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, "+"s)
	* utd::make(""s, " "s, "\n"s)
	, data, left, right, space)
{
	ast::block_raw result;

	std::string text = "<%"s + left + space + "raw"s + "%>"s + data + "<%"s + space + "endraw" + right + "%>"s;
	BOOST_TEST_CONTEXT("TEXT=" << text) {
		BOOST_REQUIRE_NO_THROW( result = cppjinja::text::parse(txt::block_raw, text) );
		BOOST_TEST( result.value == data );
		BOOST_TEST( !left.empty() == result.left_open.trim );
		BOOST_TEST( !right.empty() == result.right_close.trim );
	}
}
BOOST_AUTO_TEST_SUITE_END() // block_raw

