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
#include <boost/ptr_container/ptr_vector.hpp>

#include "mocks.hpp"
#include "evtree/node.hpp"
#include "evtree/nodes/tmpl.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using evt_node = cppjinja::evt::node;
namespace evtnodes = cppjinja::evtnodes;
namespace ast = cppjinja::ast;

boost::ptr_vector<evt_node> all_nodes()
{
	using namespace cppjinja::evtnodes;
	return {
	};
}

struct mock_exenv_fixture
{
	mocks::exenv env;
	mocks::context ctx;
	mocks::callstack calls;

	void expect_calls(evt_node* caller, cppjinja::evtnodes::callable* calling)
	{
		mock::sequence seq;
		MOCK_EXPECT(env.calls).once().in(seq).returns(calls);
		MOCK_EXPECT(calls.push).once().in(seq).with(caller, calling);
		MOCK_EXPECT(calls.pop).once().in(seq).with(caller);
	}

	void expect_cxt_settings(evt_node* maker)
	{
		mock::sequence seq;
		MOCK_EXPECT(env.get_ctx).once().in(seq).returns(ctx);
		MOCK_EXPECT(ctx.push).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(maker);
		MOCK_EXPECT(env.current_node).once().with(maker);
	}

	void expect_children(std::vector<const evt_node*> children)
	{
		MOCK_EXPECT(env.children).once().returns(children);
	}
};

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
	expect_calls(&tmpl, &child_empty_name);
	MOCK_EXPECT(child_empty_name.name).returns("");
	MOCK_EXPECT(child_with_name.name).returns("tn");
	MOCK_EXPECT(child_empty_name.render).once();
	BOOST_CHECK_NO_THROW(tmpl.render(env));
}
BOOST_AUTO_TEST_SUITE_END() // tmpl
BOOST_AUTO_TEST_SUITE_END() // nodes
