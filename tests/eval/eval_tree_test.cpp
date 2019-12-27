/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator tree

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "evaluator/evaluate_tree.hpp"

#include "parser/grammar/tmpls.hpp"

#include "mocks.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;
namespace east = cppjinja::east;
using namespace std::literals;

BOOST_AUTO_TEST_CASE(main_fnc)
{
	ast::file file = txt::parse(txt::file, "simple text"sv);
	BOOST_REQUIRE_NO_THROW( cppjinja::evaluate_tree tree(file.tmpls) );
	cppjinja::evaluate_tree tree(file.tmpls);
	std::vector<const cppjinja::eval_tree::tmpl*> tmpls;
	BOOST_CHECK_NO_THROW( tmpls = tree.all_tmpls() );
	BOOST_TEST_REQUIRE( tmpls.size() == 1);
	BOOST_TEST( tmpls[0]->name().empty() );
	BOOST_TEST( tmpls[0]->child_tmpls().empty() );

	auto blocks = tmpls[0]->child_blocks();
	BOOST_TEST_REQUIRE(
	        blocks.size() == 1,
	        "all content is a main block, block count is " << blocks.size() );
	BOOST_TEST_REQUIRE( blocks[0]->name().has_value() );
	BOOST_TEST(
	        blocks[0]->name()->empty(),
	        "main block's name is empty string, but it is "
	        << *blocks[0]->name() );
}

BOOST_AUTO_TEST_CASE(few_blocks)
{
	ast::file file = txt::parse(txt::file, "cnt<% block a %>in<%block b%>bin<%endblock%><%endblock%>"sv);
	BOOST_REQUIRE_NO_THROW( cppjinja::evaluate_tree tree(file.tmpls) );
	cppjinja::evaluate_tree tree(file.tmpls);
	std::vector<const cppjinja::eval_tree::tmpl*> tmpls;
	BOOST_CHECK_NO_THROW( tmpls = tree.all_tmpls() );
	BOOST_TEST_REQUIRE( tmpls.size() == 1);
	BOOST_TEST( tmpls[0]->name().empty() );
	BOOST_TEST( tmpls[0]->child_tmpls().empty() );

	auto blocks = tmpls[0]->child_blocks();
	for(auto& b:blocks) BOOST_TEST_REQUIRE( b->name().has_value() );
	BOOST_TEST_REQUIRE( blocks.size() == 3 );
	BOOST_TEST(  blocks[2]->name()->empty() );
	BOOST_TEST( !blocks[1]->name()->empty() );
	BOOST_TEST( !blocks[0]->name()->empty() );

	const cppjinja::eval_tree::block* cur_bl =
	        tree.find_block(ast::var_name{"a"s}, blocks[0]);
	BOOST_TEST_REQUIRE( cur_bl != nullptr );
	BOOST_TEST( *cur_bl->name() == "a"s );

	cur_bl = tree.find_block(ast::var_name{"b"s}, blocks[0]);
	BOOST_TEST_REQUIRE( cur_bl != nullptr );
	BOOST_TEST( *cur_bl->name() == "b"s );
}

BOOST_AUTO_TEST_CASE(extends)
{
	ast::file file = txt::parse(txt::file,
	                            "<% template base %>main"
	                            "<%block ba%>bain<%endblock%>"
	                            "<%endtemplate%>"
	                            "<%template child extends base%>"
	                            "child"
	                            "<%block ca%>cain<%endblock%>"
	                            "<%endtemplate%>"sv);
	BOOST_REQUIRE_NO_THROW( cppjinja::evaluate_tree tree(file.tmpls) );
	cppjinja::evaluate_tree tree(file.tmpls);
	std::vector<const cppjinja::eval_tree::tmpl*> tmpls;
	BOOST_CHECK_NO_THROW( tmpls = tree.all_tmpls() );
	BOOST_TEST_REQUIRE( tmpls.size() == 1);
	BOOST_TEST( tmpls[0]->name() == "base" );
	BOOST_TEST_REQUIRE( tmpls[0]->child_tmpls().size() == 1 );
	BOOST_TEST( tmpls[0]->child_tmpls()[0]->name() == "child" );

	auto blocks = tmpls[0]->child_blocks();
	for(auto& b:blocks) BOOST_TEST_REQUIRE( b->name().has_value() );
	BOOST_TEST_REQUIRE( blocks.size() == 2 );
	blocks = tmpls[0]->child_tmpls()[0]->child_blocks();
	for(auto& b:blocks) BOOST_TEST_REQUIRE( b->name().has_value() );
	BOOST_TEST_REQUIRE( blocks.size() == 2 );

	const cppjinja::eval_tree::block* cur_bl =
	        tree.find_block(ast::var_name{"ba"s}, blocks[0]);
	BOOST_TEST_REQUIRE( cur_bl != nullptr );
	BOOST_TEST( *cur_bl->name() == "ba"s );
}
