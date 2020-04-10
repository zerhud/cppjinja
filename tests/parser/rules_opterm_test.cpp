/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_opterm

#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/operators/opterm.hpp"
#include "parser/grammar/opterm.hpp"

using namespace std::literals;
namespace txt = cppjinja::text;
namespace ast = cppjinja::ast;
namespace tdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(parser)
BOOST_AUTO_TEST_SUITE(op_term)

BOOST_AUTO_TEST_CASE(common)
{
	ast::op_term_start result;

	std::string data = "<=";
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_term_start, data) );
	BOOST_TEST( !result.trim );

	data = "<=+";
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_term_start, data) );
	BOOST_TEST( result.trim );

	data = "<%";
	BOOST_REQUIRE_THROW( result = txt::parse(txt::op_term_start, data), std::exception );

	data = "{=+";
	cppjinja::parser_env env; env.output.b="{=";
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::op_term_start, data, std::move(env)) );
	BOOST_TEST( result.trim );
}

BOOST_AUTO_TEST_CASE(op_block)
{
	ast::block_term_start result;

	std::string data = "<%";
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_term_start, data) );
	BOOST_TEST( !result.trim );
}

BOOST_AUTO_TEST_CASE(no_space)
{
	std::string data = "< %";
	BOOST_CHECK_THROW( txt::parse(txt::block_term_start, data), std::exception );
}

BOOST_AUTO_TEST_SUITE(bsign)
BOOST_DATA_TEST_CASE(begin, tdata::random(-100, 100) ^ tdata::xrange(10), sign, ind)
{
	// TODO: 10'000'000'000 iterations blocks the system.. memory leaks?
	(void)ind;
	std::string data = "<%" + (0 <= sign ? ('+' + std::to_string(sign)) : std::to_string(sign));
	ast::block_term_start result;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_term_start, data) );
	BOOST_TEST( result.trim );
	BOOST_CHECK( result.bsign );
	BOOST_TEST( *result.bsign == sign );
}
BOOST_DATA_TEST_CASE(end, tdata::random(-100, 100) ^ tdata::xrange(10), sign, ind)
{
	(void)ind;
	std::string data = (0 < sign ? ('+' + std::to_string(sign)) : std::to_string(sign)) + "%>"s;
	ast::block_term_end result;
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::block_term_end, data) );
	BOOST_TEST( result.trim );
	BOOST_CHECK( result.bsign );
	BOOST_TEST( *result.bsign == sign );
}
BOOST_AUTO_TEST_CASE(only_block)
{
	// start cannot be checke here: the tail is ommited
	BOOST_CHECK_THROW( txt::parse(txt::op_term_end, "+1=>"s), std::exception );
	BOOST_CHECK_THROW( txt::parse(txt::comment_term_end, "+1#>"s), std::exception );
}
BOOST_AUTO_TEST_SUITE_END() // bsign

BOOST_AUTO_TEST_SUITE_END() // parser
BOOST_AUTO_TEST_SUITE_END() // op_term
