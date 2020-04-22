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
#include "evtree/evtree.hpp"

#include "parser/grammar/tmpls.hpp"

#include "mocks.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;
namespace east = cppjinja::east;
using namespace std::literals;

cppjinja::evtree build_tree(std::string_view ptxt)
{
	ast::file file;
	BOOST_TEST_CONTEXT("parsing: " << ptxt)
	    BOOST_REQUIRE_NO_THROW( file = txt::parse(txt::file, ptxt) );

	cppjinja::evtree tree;
	for(auto& t:file.tmpls) BOOST_CHECK_NO_THROW( tree.add_tmpl(t) );
	return tree;
}

BOOST_AUTO_TEST_CASE(main_fnc)
{
	cppjinja::evtree tree = build_tree("simple text"sv);
	BOOST_TEST(tree.print_subtree() == "''[''['content'[]]]");
	BOOST_TEST( tree.search(ast::var_name{"nex"}, nullptr) == nullptr );
}

BOOST_AUTO_TEST_CASE(few_blocks)
{
	cppjinja::evtree tree = build_tree(
	          "cnt<% block a %>in<%block b%>bin<%endblock%><%endblock%>"sv);

	const cppjinja::evt::node* a_node =
	        tree.search(ast::var_name{""s, "a"s}, nullptr);
	const cppjinja::evt::node* b_node =
	        tree.search(ast::var_name{""s, "b"s}, nullptr);
	BOOST_TEST_REQUIRE(a_node);
	BOOST_TEST_REQUIRE(b_node);
	BOOST_TEST( a_node->name() == "a"s );
	BOOST_TEST( b_node->name() == "b"s );


	auto a_children = tree.children(a_node);
	BOOST_TEST_REQUIRE(a_children.size() == 2);
	BOOST_TEST(a_children[0]->name() == "content");
	BOOST_TEST(a_children[1]->name() == "b");
}

BOOST_AUTO_TEST_CASE(extends)
{
	cppjinja::evtree tree = build_tree(
	                            "<% template base %>main"
	                            "<%block ba%>bain<%endblock%>"
	                            "<%endtemplate%>"
	                            "<%template child extends base%>"
	                            "child"
	                            "<%block ca%>cain<%endblock%>"
	                            "<%endtemplate%>"sv);
	auto nodes = tree.all_tree();

	auto* tmpl_base_node = tree.search_tmpl(ast::var_name{"base"s});
	BOOST_REQUIRE( tmpl_base_node );
	BOOST_TEST( tmpl_base_node->name() == "base" );

	const cppjinja::evt::node* tmpl_child_node =
	        tree.search_tmpl(ast::var_name{"child"s});
	BOOST_TEST_REQUIRE( tmpl_child_node != nullptr );
	BOOST_TEST( tmpl_child_node->name() == "child"s );

	auto base_main = tree.search(ast::var_name{""s}, tmpl_base_node);
	BOOST_TEST_REQUIRE( base_main != nullptr );
	BOOST_TEST( base_main->name().empty() );

	auto child_ca = tree.search(ast::var_name{"child"s, "ca"s});
	BOOST_TEST_REQUIRE( child_ca != nullptr );
	BOOST_TEST( child_ca->name() == "ca"s );
}

