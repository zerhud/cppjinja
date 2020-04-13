/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <turtle/mock.hpp>

#include "eval/eval.hpp"
#include "evtree/nodes/callable.hpp"
#include "evtree/exenv/context.hpp"
#include "evtree/exenv/callstack.hpp"
#include "parser/operators/single.hpp"

namespace mocks {

MOCK_BASE_CLASS( data_provider, cppjinja::data_provider)
{
	MOCK_METHOD( value, 1, cppjinja::east::value_term(const cppjinja::east::var_name& val), value_var_name )
	MOCK_METHOD( value, 1, cppjinja::east::value_term(const cppjinja::east::function_call& val), value_function_call )
	MOCK_METHOD( filter, 2 )
};

MOCK_BASE_CLASS( node, cppjinja::evt::node )
{
	MOCK_METHOD( rinfo, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( is_leaf, 0 )
	MOCK_METHOD( render, 1 )
};

MOCK_BASE_CLASS( callable_node, cppjinja::evtnodes::callable )
{
	MOCK_METHOD( rinfo, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( is_leaf, 0 )
	MOCK_METHOD( render, 1 )
	MOCK_METHOD( param, 2, std::optional<cppjinja::ast::value_term>(const cppjinja::evt::callstack& ctx,const cppjinja::ast::var_name& name) )
};

MOCK_BASE_CLASS( context, cppjinja::evt::context )
{
	MOCK_METHOD(current_node, 1)
	MOCK_METHOD(nth_node_on_stack, 1)

	MOCK_METHOD(pop, 1)
	MOCK_METHOD(push, 1)
	MOCK_METHOD(maker, 0)

	MOCK_METHOD(inject, 1)
	MOCK_METHOD(takeout, 1)
	MOCK_METHOD(injections, 0)

	MOCK_METHOD(solve_var, 1)
};

MOCK_BASE_CLASS( callstack, cppjinja::evt::callstack )
{
	using cppjinja::evt::callstack::call_params;
	MOCK_METHOD( pop, 1 )
	MOCK_METHOD( push, 2 )
	MOCK_METHOD( caller, 0 )
	MOCK_METHOD( calling_stack, 0 )
	// get_params
	MOCK_METHOD( call_params, 0, std::vector<cppjinja::ast::function_call_parameter>(), get_params )
	// set_params
	MOCK_METHOD( call_params, 1, void (std::vector<cppjinja::ast::function_call_parameter> params), set_params )
};

MOCK_BASE_CLASS( exenv, cppjinja::evt::exenv )
{
	MOCK_METHOD(tmpl, 0)
	MOCK_METHOD(children, 1)
	MOCK_METHOD(search_callable, 1)

	MOCK_METHOD(data, 0)
	MOCK_METHOD(out, 0)

	using cppjinja::evt::exenv::ctx;
	MOCK_CONST_METHOD( ctx, 0, const context&(), get_cctx )
	MOCK_NON_CONST_METHOD( ctx, 0, context&(), get_ctx )

	MOCK_METHOD( current_node, 1)

	using cppjinja::evt::exenv::calls;
	MOCK_NON_CONST_METHOD(calls, 0, callstack&(), calls)
	MOCK_CONST_METHOD( calls, 0, const callstack&(), ccalls )

	using cppjinja::evt::exenv::rinfo;
	MOCK_CONST_METHOD(rinfo, 0, std::optional<cppjinja::evt::render_info>(), crinfo)
	MOCK_NON_CONST_METHOD(rinfo, 1, void(std::optional<cppjinja::evt::render_info> ri), rinfo)
};

struct mock_exenv_fixture
{
	mocks::exenv env;
	mocks::context ctx;
	mocks::callstack calls;
	std::stringstream out;
	mocks::data_provider data;

	mock_exenv_fixture()
	{
		MOCK_EXPECT(env.data).returns(&data);
		MOCK_EXPECT(env.get_ctx).returns(ctx);
		MOCK_EXPECT(env.get_cctx).returns(ctx);
		MOCK_EXPECT(env.calls).returns(calls);
		MOCK_EXPECT(env.ccalls).returns(calls);
		MOCK_EXPECT(env.out).calls([this]()->std::ostream&{return out;});
	}

	void expect_callings(std::vector<const cppjinja::evtnodes::callable*> stack)
	{
		MOCK_EXPECT(calls.calling_stack).once().returns(stack);
	}

	void expect_call(
	        cppjinja::evt::node* caller,
	        cppjinja::evtnodes::callable* calling,
	        std::vector<cppjinja::ast::function_call_parameter> params)
	{
		auto check_params =
		        [params](std::vector<cppjinja::ast::function_call_parameter> called_params){
			BOOST_REQUIRE(called_params.size() == params.size());
			BOOST_TEST( called_params == params );
		};
		mock::sequence seq;
		MOCK_EXPECT(calls.push).once().in(seq).with(caller, calling);
		MOCK_EXPECT(calls.set_params)
		        .at_most(1)
		        .at_least(params.empty() ? 0 : 1)
		        .in(seq).calls(check_params);
		MOCK_EXPECT(calls.pop).once().in(seq);
	}

	void expect_cxt_settings(cppjinja::evt::node* maker)
	{
		mock::sequence seq;
		MOCK_EXPECT(ctx.push).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(maker);
		MOCK_EXPECT(env.current_node).once().with(maker);
	}

	void expect_children(std::vector<const cppjinja::evt::node*> children)
	{
		MOCK_EXPECT(env.children).once().returns(children);
	}
};

} // namespace mocks
