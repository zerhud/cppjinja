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

BOOST_AUTO_TEST_SUITE(evaltree)


BOOST_AUTO_TEST_CASE(few_blocks)
{
	cppjinja::evtree tree = build_tree(
	          "cnt<% block a %>in<%block b%>bin<%endblock%><%endblock%>"sv);

	const cppjinja::evt::node* a_node;
	const cppjinja::evt::node* b_node;
	auto roots = tree.roots(tree.search_tmpl(""));
	for(auto& r:roots) {
		if(r->name() == "a") a_node = r;
		else if(r->name() == "b") b_node = r;
	}

	BOOST_TEST_REQUIRE(a_node);
	BOOST_TEST_REQUIRE(b_node);
	BOOST_TEST( a_node->name() == "a"s );
	BOOST_TEST( b_node->name() == "b"s );


	auto a_children = tree.children(a_node);
	BOOST_TEST_REQUIRE(a_children.size() == 2);
	BOOST_TEST(a_children[0]->name() == "content");
	BOOST_TEST(a_children[1]->name() == "b");
}

BOOST_AUTO_TEST_CASE(roots)
{
	cppjinja::evtree tree = build_tree(
	          "cnt<% block a %>in<%block b%>bin<%endblock%><%endblock%>"sv);
	const cppjinja::evtnodes::tmpl* tmpl = tree.search_tmpl("");
	BOOST_TEST(tree.roots(nullptr).size() == 0);
	BOOST_TEST(tree.roots(tmpl).size() == 3);
	for(auto& r:tree.roots(tmpl))
		BOOST_CHECK(r->name().empty() || r->name() == "a" || r->name() == "b");
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

	auto* tmpl_base_node = tree.search_tmpl("base"s);
	BOOST_REQUIRE( tmpl_base_node );
	BOOST_TEST( tmpl_base_node->name() == "base" );

	const cppjinja::evt::node* tmpl_child_node =
	        tree.search_tmpl("child"s);
	BOOST_TEST_REQUIRE( tmpl_child_node != nullptr );
	BOOST_TEST( tmpl_child_node->name() == "child"s );

	auto roots = tree.roots(tree.search_tmpl("base"));
	BOOST_TEST(roots.size() == 2);
	for(auto& r:roots)
		BOOST_CHECK(r->name() == "" || r->name() == "ba");

	roots = tree.roots(tree.search_tmpl("child"));
	BOOST_TEST(roots.size() == 2);
	for(auto& r:roots)
		BOOST_CHECK(r->name() == "" || r->name() == "ca");
}

BOOST_AUTO_TEST_SUITE_END() // evaltree
