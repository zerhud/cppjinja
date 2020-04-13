/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator nodes

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/node.hpp"
#include "evtree/nodes/tmpl.hpp"
#include "evtree/nodes/op_set.hpp"
#include "evtree/nodes/op_out.hpp"
#include "evtree/nodes/content_block.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using evt_node = cppjinja::evt::node;
namespace evtnodes = cppjinja::evtnodes;
namespace ast = cppjinja::ast;
using ast::value_term;
using mocks::mock_exenv_fixture; // qtcreator cannot parse test with namespace

template<typename Node, typename Ast>
void test_name_equals(const Node& evtn, const Ast& astnode)
{
	BOOST_TEST( evtn.name() == astnode.name );
}

template<typename Node, typename Ast>
void test_block_nodes_rinfo(const Node& evtn, const Ast& astnode)
{
	//evtn.rinfo();
}

BOOST_AUTO_TEST_SUITE(nodes)
BOOST_AUTO_TEST_SUITE(tmpl)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::tmpl t;
	t.name = "test_name";
	evtnodes::tmpl tmpl(t);
	BOOST_CHECK( !tmpl.rinfo().trim_left );
	BOOST_CHECK( !tmpl.rinfo().trim_right );
	BOOST_CHECK( !tmpl.is_leaf() );
	test_name_equals( tmpl, t );
}
BOOST_FIXTURE_TEST_CASE(no_children_no_render, mock_exenv_fixture)
{
	expect_children({});
	evtnodes::tmpl tmpl(ast::tmpl{});
	expect_cxt_settings(&tmpl);
	BOOST_CHECK_NO_THROW(tmpl.render(env));
}
BOOST_FIXTURE_TEST_CASE(rendered_only_empty_name, mock_exenv_fixture)
{
	evtnodes::tmpl tmpl(ast::tmpl{});
	expect_cxt_settings(&tmpl);
	mocks::callable_node child_with_name, child_empty_name;
	expect_children({&child_with_name, &child_empty_name});
	expect_call(&tmpl, &child_empty_name, {});
	MOCK_EXPECT(child_empty_name.name).returns("");
	MOCK_EXPECT(child_with_name.name).returns("tn");
	MOCK_EXPECT(child_empty_name.render).once();
	BOOST_CHECK_NO_THROW(tmpl.render(env));
}
BOOST_AUTO_TEST_SUITE_END() // tmpl
BOOST_AUTO_TEST_SUITE(op_set)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	BOOST_TEST(snode.name() == "tname");
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
	BOOST_TEST(snode.is_leaf() == true);
}
BOOST_AUTO_TEST_CASE(value)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	BOOST_TEST(snode.value(ast::var_name{"tname"}) == value_term{42});
	BOOST_CHECK_THROW(snode.value(ast::var_name{"wrong_name"}), std::exception);
	BOOST_CHECK_THROW(snode.value(ast::var_name{}), std::exception);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	BOOST_CHECK_NO_THROW(snode.render(env));
}
BOOST_AUTO_TEST_SUITE_END() // op_set
BOOST_AUTO_TEST_SUITE(op_out)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::op_out ast_out{ {1,1}, value_term{42}, {}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_out snode(ast_out);
	BOOST_TEST(snode.name().substr(0,6) == "op_out");
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
	BOOST_TEST(snode.is_leaf() == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	ast::op_out ast_out{ {1,1}, value_term{42}, {}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_out snode(ast_out);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	BOOST_CHECK_NO_THROW(snode.render(env));
}
BOOST_FIXTURE_TEST_CASE(render_with_filters, mock_exenv_fixture)
{
	ast::op_out ast_out{ {1,1}, value_term{42}, {}, {{1,1},false}, {{1,1},true} };
	ast_out.filters.push_back(ast::filter_call{ast::var_name{"a"}});
	ast_out.filters.push_back(ast::filter_call{ast::function_call(ast::var_name{"fnc"}, {})});
	evtnodes::op_out snode(ast_out);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	mock::sequence filter_seq;
	MOCK_EXPECT(data.filter).once().in(filter_seq).returns("first");
	MOCK_EXPECT(data.filter).once().in(filter_seq).returns("ok");
	MOCK_EXPECT(calls.set_params).once().calls([](auto p){BOOST_CHECK(p.empty());});
	BOOST_CHECK_NO_THROW(snode.render(env));
	BOOST_TEST(out.str() == "ok");
}
BOOST_AUTO_TEST_SUITE_END() // op_out
BOOST_AUTO_TEST_SUITE(content_block)
BOOST_FIXTURE_TEST_CASE(getters, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true}, "test_name");
	BOOST_TEST(cnt_bl.is_leaf() == false);
	BOOST_TEST(cnt_bl.name() == "test_name");
	BOOST_TEST(cnt_bl.rinfo().trim_left == false);
	BOOST_TEST(cnt_bl.rinfo().trim_right == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true}, "test_name");
	expect_cxt_settings(&cnt_bl);
	MOCK_EXPECT(env.crinfo).once().returns(cppjinja::evt::render_info{true, false});
	MOCK_EXPECT(env.children).once().returns(std::vector<const evt_node*>{});
	BOOST_CHECK_NO_THROW(cnt_bl.render(env));
}
BOOST_FIXTURE_TEST_CASE(render_two, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true}, "test_name");
	mocks::node child1, child2;
	expect_cxt_settings(&cnt_bl);
	MOCK_EXPECT(env.crinfo).returns(cppjinja::evt::render_info{true, false});
	MOCK_EXPECT(env.children).once().returns(std::vector<const evt_node*>{&child1, &child2});
	MOCK_EXPECT(child1.render).once();
	MOCK_EXPECT(child1.rinfo).once().returns(cppjinja::evt::render_info{false,false});
	MOCK_EXPECT(child2.render).once();
	MOCK_EXPECT(child2.rinfo).once().returns(cppjinja::evt::render_info{false,false});
	MOCK_EXPECT(env.rinfo).exactly(2);
	BOOST_CHECK_NO_THROW(cnt_bl.render(env));
}
BOOST_AUTO_TEST_SUITE_END() // content_block
BOOST_AUTO_TEST_SUITE_END() // nodes
