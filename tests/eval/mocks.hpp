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
#include "evtree/exenv/context_object.hpp"
#include "parser/operators/single.hpp"
#include "evtree/exenv/context_objects/value.hpp"

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
	MOCK_METHOD( render, 1 )
};

MOCK_BASE_CLASS( callable_node, cppjinja::evtnodes::callable )
{
	MOCK_METHOD( rinfo, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( render, 1 )
	MOCK_METHOD( evaluate, 1 )
	MOCK_METHOD( solved_params, 1 )
};

MOCK_BASE_CLASS( context, cppjinja::evt::context )
{
	MOCK_METHOD(current_node, 1)
	MOCK_METHOD(nth_node_on_stack, 1)

	MOCK_METHOD(pop, 1)
	MOCK_METHOD(push, 1)
	MOCK_METHOD(maker, 0)

	MOCK_METHOD(out, 0)
	MOCK_METHOD(result, 0)

	MOCK_METHOD(cur_namespace, 0)
};

MOCK_BASE_CLASS(context_object, cppjinja::evt::context_object)
{
	MOCK_METHOD(add, 2)
	MOCK_METHOD(find, 1)
	MOCK_METHOD(solve, 0)
	MOCK_METHOD(call, 1)
};

MOCK_BASE_CLASS( callstack, cppjinja::evt::callstack )
{
	MOCK_METHOD( push, 2 )
	MOCK_METHOD( pop, 0 )
	MOCK_METHOD( current_params, 1 )
};

MOCK_BASE_CLASS( exenv, cppjinja::evt::exenv )
{
	MOCK_METHOD(tmpl, 0)
	MOCK_METHOD(children, 1)
	MOCK_METHOD(roots, 1)

	MOCK_METHOD(data, 0)
	MOCK_METHOD(out, 0)
	MOCK_METHOD(result, 0)

	using cppjinja::evt::exenv::ctx;
	MOCK_CONST_METHOD( ctx, 0, const context&(), get_cctx )
	MOCK_NON_CONST_METHOD( ctx, 0, context&(), get_ctx )

	MOCK_METHOD( current_node, 1)
	MOCK_METHOD( globals, 0)
	MOCK_METHOD( locals, 0)
	MOCK_METHOD( params, 0)
	MOCK_METHOD( user_data, 0)
	MOCK_METHOD( all_ctx, 0)

	MOCK_NON_CONST_METHOD(calls, 0, callstack&(), calls)

	using cppjinja::evt::exenv::rinfo;
	MOCK_CONST_METHOD(rinfo, 0, std::optional<cppjinja::evt::render_info>(), crinfo)
	MOCK_NON_CONST_METHOD(rinfo, 1, void(std::optional<cppjinja::evt::render_info> ri), rinfo)
	MOCK_METHOD( render_format, 0)
};

struct mock_exenv_fixture
{
	mocks::exenv env;
	mocks::context ctx;
	mocks::callstack calls;
	std::stringstream out;
	mocks::data_provider data;
	cppjinja::evt::result_formatter rfmt;

	context_object params, locals, globals, user_data, all_ctx;
	std::shared_ptr<context_object> mock_all_ctx = std::make_shared<context_object>();

	mock_exenv_fixture() : params({})
	{
		MOCK_EXPECT(env.data).returns(&data);
		MOCK_EXPECT(env.get_ctx).returns(ctx);
		MOCK_EXPECT(env.get_cctx).returns(ctx);
		MOCK_EXPECT(env.calls).returns(calls);
		MOCK_EXPECT(env.out).calls([this]()->std::ostream&{return out;});
		MOCK_EXPECT(env.render_format).calls([this]()->cppjinja::evt::result_formatter&{return rfmt;});
		MOCK_EXPECT(env.all_ctx).calls([this]()->cppjinja::evt::context_objects::queue{
		                                   return {&all_ctx};
		                               });
	}

	void expect_glp(int gc, int lc, int pc)
	{
		using cppjinja::evt::context_object;
		using cppjinja::evt::context_objects::queue;
		MOCK_EXPECT(env.params).at_least(pc).calls([this]()->queue{return queue({&params});});
		MOCK_EXPECT(env.locals).at_least(lc).calls([this]()->context_object&{return locals;});
		MOCK_EXPECT(env.globals).at_least(gc).calls([this]()->context_object&{return globals;});
		MOCK_EXPECT(env.user_data).calls([this]()->context_object&{return user_data;});
	}

	void expect_sovle(cppjinja::east::var_name n, cppjinja::east::value_term v)
	{
		MOCK_EXPECT(all_ctx.find).once().with(n).returns(mock_all_ctx);
		MOCK_EXPECT(mock_all_ctx->solve).once().returns(v);
	}

	void expect_call(cppjinja::east::var_name n, std::vector<cppjinja::east::function_parameter> p, cppjinja::east::value_term v)
	{
		auto val_obj= std::make_shared<cppjinja::evt::context_objects::value>(std::move(v));
		MOCK_EXPECT(all_ctx.find).once().with(n).returns(mock_all_ctx);
		MOCK_EXPECT(mock_all_ctx->call).once().with(p).returns(val_obj);
	}

	void expect_call(cppjinja::evtnodes::callable* calling)
	{
		mock::sequence seq;
		MOCK_EXPECT(calls.push).once().with(calling, mock::any).in(seq);
		MOCK_EXPECT(calls.pop).once().in(seq);
	}

	void expect_cxt_settings(cppjinja::evt::node* maker)
	{
		mock::sequence seq;
		MOCK_EXPECT(env.current_node).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.push).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(maker);
	}

	void expect_tmpl_cxt_settings(cppjinja::evt::node* maker)
	{
		mock::sequence seq;
		MOCK_EXPECT(ctx.push).once().in(seq).with(maker);
		MOCK_EXPECT(env.current_node).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(maker);
	}

	void expect_children(std::vector<const cppjinja::evt::node*> children)
	{
		MOCK_EXPECT(env.children).at_least(1).returns(children);
	}

	void expect_roots(std::vector<const cppjinja::evtnodes::callable*> roots)
	{
		MOCK_EXPECT(env.roots).at_least(1).returns(roots);
	}
};

} // namespace mocks
