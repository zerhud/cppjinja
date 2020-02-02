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
	ast::file file = txt::parse(txt::file, ptxt);
	cppjinja::evtree tree;
	for(auto& t:file.tmpls) BOOST_CHECK_NO_THROW( tree.add_tmpl(t) );
	return tree;
}

BOOST_AUTO_TEST_CASE(main_fnc)
{
	cppjinja::evtree tree = build_tree("simple text"sv);
	auto nodes = tree.all_tree();

	BOOST_TEST_REQUIRE( nodes.size() == 3);
	BOOST_TEST( nodes[0]->name().empty() );
	BOOST_TEST( nodes[1]->name().empty() );
	BOOST_TEST( nodes[1]->is_parent(nodes[0]) );
	BOOST_TEST( nodes[2]->name() == "content" );
	BOOST_TEST( nodes[2]->is_parent(nodes[1]) );
	BOOST_TEST( tree.search(ast::var_name{"nex"}, nullptr) == nullptr );
}

BOOST_AUTO_TEST_CASE(few_blocks)
{
	cppjinja::evtree tree = build_tree(
	            "cnt<% block a %>in<%block b%>bin<%endblock%><%endblock%>"sv);
	auto nodes = tree.all_tree();

	BOOST_TEST_REQUIRE( nodes.size() == 9 );

	const cppjinja::evt::node* a_node =
	        tree.search(ast::var_name{""s, "a"s}, nullptr);
	const cppjinja::evt::node* b_node =
	        tree.search(ast::var_name{""s, "b"s}, nullptr);

	BOOST_TEST_REQUIRE(a_node);
	BOOST_TEST_REQUIRE(b_node);

	const cppjinja::evt::node* tmpl = nodes[0];

	BOOST_TEST( nodes[1]->is_parent(tmpl) );
	BOOST_TEST( nodes[2]->is_parent(nodes[1]) );
	BOOST_TEST( a_node->is_parent(tmpl) );

	BOOST_TEST( a_node->name() == "a"s );
	BOOST_TEST( b_node->name() == "b"s );
	BOOST_TEST( nodes[1]->name().empty(),
	        "block is ordered as is, but the last block is the main, but it is "
	        << nodes[3]->name()
	        );

	BOOST_TEST( nodes[4]->name() == "content"s );
	BOOST_TEST( nodes[6]->name() == "content"s );
	BOOST_TEST( a_node == nodes[3] );
	BOOST_TEST( b_node == nodes[5] );
	BOOST_TEST( tree.search(ast::var_name{""s,  ""s}, nullptr) == nodes[1] );

	BOOST_TEST( tree.search(ast::var_name{"a"s}, nodes[2]) == a_node );
	BOOST_TEST( tree.search(ast::var_name{"b"s}, nodes[2]) == b_node );
	BOOST_TEST( tree.search(ast::var_name{"b"s}, nodes[1]) == nodes[5] );

	auto a_children = tree.children(a_node);
	BOOST_TEST_REQUIRE(a_children.size() == 2);
	BOOST_TEST(a_children[0]->name() == "content");
	BOOST_TEST(a_children[1]->name() == "op_out");
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

	BOOST_TEST_REQUIRE( nodes.size() == 12 );
	BOOST_TEST( tree.search(ast::var_name{"base"s}) == nodes[0] );

	const cppjinja::evt::node* base = nodes[0];
	BOOST_TEST_REQUIRE( base != nullptr );
	BOOST_TEST( !base->is_leaf() );
	BOOST_TEST( base->name() == "base"s );

	const cppjinja::evt::node* child = tree.search(ast::var_name{"child"s});
	BOOST_TEST_REQUIRE( child != nullptr );
	BOOST_TEST( !child->is_leaf() );
	BOOST_TEST( child->name() == "child"s );
	BOOST_TEST( child->is_parent(base) );

	auto base_blocks = tree.children(base);
	BOOST_TEST_REQUIRE( base_blocks.size() == 11,
	                       "block children is main blocks, template child and "
	                    << "all blocks, but it's count is "
	                    << base_blocks.size()
	                    );
	auto base_main = tree.search(ast::var_name{""s}, base_blocks[0]);
	BOOST_TEST_REQUIRE( base_main != nullptr );
	BOOST_TEST( base_main->name().empty() );

	auto base_ba = tree.search(ast::var_name{"ba"s}, base_main);
	BOOST_TEST_REQUIRE( base_ba != nullptr );
	BOOST_TEST( base_ba->name() == "ba"s );

	auto child_ca = tree.search(ast::var_name{"child"s, "ca"s});
	BOOST_TEST_REQUIRE( child_ca != nullptr );
	BOOST_TEST( child_ca->name() == "ca"s );

	BOOST_TEST( tree.search(ast::var_name{"ba"s}, child_ca) == base_ba );
}

BOOST_AUTO_TEST_CASE(tree_blocks)
{
	cppjinja::evtree tree = build_tree("<%block bl%>cnt<%endblock%>");
	auto nodes = tree.all_tree();

	const cppjinja::evt::node* bl = nullptr;
	const cppjinja::evt::node* main = nullptr;
	for(auto& n:nodes)
	{
		if(n->name()=="bl") bl = n;
		if(n->name().empty()) main = n;
	}

	BOOST_TEST_REQUIRE( bl );
	BOOST_TEST_REQUIRE( main );

	BOOST_TEST( tree.children(bl).size()==1 );
	auto children = tree.children(main);
	BOOST_TEST_REQUIRE(children.size()==1);
	BOOST_TEST(children[0]->name()=="op_out");
}
