/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <memory_resource>
#include <turtle/mock.hpp>

#include "eval/eval.hpp"
#include "evtree/nodes/callable.hpp"
#include "evtree/exenv/context.hpp"
#include "evtree/exenv/callstack.hpp"
#include "evtree/exenv/context_object.hpp"
#include "parser/operators/single.hpp"
#include "evtree/exenv/context_objects/value.hpp"

#include "absd/simple_data_holder.hpp"


absd::data operator "" _ad(const char* src, std::size_t size)
{
	return absd::data{std::make_shared<absd::simple_data_holder>(
		            std::pmr::string(src,size))};
}
absd::data operator "" _ad(unsigned long long int val)
{
	return absd::data{std::make_shared<absd::simple_data_holder>(val)};
}

std::pmr::string operator "" _s(const char* src, std::size_t size)
{
	return std::pmr::string(src, size);
}

namespace std {
bool operator == (const std::string& l, const std::pmr::string& r)
{
	if(l.size() != r.size()) return false;
	for(std::size_t i=0;i<l.size();++i) if(l[i]!=r[i]) return false;
	return true;
}
bool operator == (const std::pmr::string& l, const std::string& r)
{
	return r == l;
}
} // namespace std

namespace mocks {

MOCK_BASE_CLASS( data_provider, cppjinja::data_provider)
{
	MOCK_METHOD( value, 1, absd::data(const cppjinja::east::var_name& val), value_var_name )
	MOCK_METHOD( value, 1, absd::data(const cppjinja::east::function_call& val), value_function_call )
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
	MOCK_METHOD(push_shadow, 1)
	MOCK_METHOD(maker, 0)
	MOCK_METHOD(current_tmpl, 0)

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

	MOCK_METHOD(storage, 0)

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
	std::shared_ptr<std::pmr::memory_resource> mem_r =
	        std::make_shared<std::pmr::unsynchronized_pool_resource>();

	context_object params, locals, globals, user_data, all_ctx;
	std::shared_ptr<context_object> mock_all_ctx = std::make_shared<context_object>();

	mock_exenv_fixture() : params({})
	{
		MOCK_EXPECT(env.storage).returns(mem_r);
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

	template<absd::AnyData T>
	void expect_local_object_created(std::string name, T d)
	{
		auto data = create_absd_data(std::move(d));
		MOCK_EXPECT(locals.add)
		        .once().with(name, mock::any)
		        .calls([data](auto, auto p){ BOOST_TEST(p->solve()==data); });
	}

	void expect_sovle(cppjinja::east::var_name n, absd::data v)
	{
		MOCK_EXPECT(all_ctx.find).once().with(n).returns(mock_all_ctx);
		MOCK_EXPECT(mock_all_ctx->solve).once().returns(v);
	}

	void expect_call(
	        cppjinja::east::var_name n,
	        std::vector<cppjinja::evt::context_object::function_parameter> p,
	        absd::data v)
	{
		using namespace std::literals;
		auto val_obj= std::make_shared<cppjinja::evt::context_objects::value>(std::move(v));
		MOCK_EXPECT(all_ctx.find).once().with(n).returns(mock_all_ctx);
		MOCK_EXPECT(mock_all_ctx->call).once().calls([val_obj,p](auto params){
			BOOST_TEST(params.size()==p.size());
			for(std::size_t i=0;i<params.size();++i) {
				BOOST_TEST(params.at(i).name.value_or(""_s) == p.at(i).name.value_or(""_s));
				BOOST_TEST(params.at(i).value->solve() == p.at(i).value->solve());
			}
			return val_obj;
		});
	}

	void expect_call(cppjinja::evtnodes::callable* calling)
	{
		mock::sequence seq;
		MOCK_EXPECT(calls.push).once().with(calling, mock::any).in(seq);
		MOCK_EXPECT(calls.pop).once().in(seq);
	}

	void expect_solved_params(
	        callable_node& node,
	        std::pmr::vector<cppjinja::east::function_parameter> params)
	{
		MOCK_EXPECT(node.solved_params).returns(params);
	}

	void expect_cxt_settings(cppjinja::evt::node* maker)
	{
		mock::sequence seq;
		MOCK_EXPECT(env.current_node).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.push_shadow).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(maker);
	}

	void expect_transporent_cxt(cppjinja::evt::node* maker)
	{
		mock::sequence seq;
		MOCK_EXPECT(env.current_node).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.push).once().in(seq).with(maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(maker);
	}

	void expect_children(std::pmr::vector<const cppjinja::evt::node*> children)
	{
		MOCK_EXPECT(env.children).at_least(1).returns(children);
	}

	template<absd::TrivialData T>
	static auto create_absd_data(T d)
	{
		auto dh = std::make_shared<absd::simple_data_holder>();
		*dh = d;
		return absd::data{dh};
	}

	static auto create_absd_data(std::pmr::string d)
	{
		auto dh = std::make_shared<absd::simple_data_holder>();
		dh->str() = d;
		return absd::data{dh};
	}

	template<absd::AnyData T>
	void expect_solve(context_object& obj, T d)
	{
		MOCK_EXPECT(obj.solve).returns(create_absd_data(d));
	}

	void expect_roots(std::pmr::vector<const cppjinja::evtnodes::callable*> roots)
	{
		MOCK_EXPECT(env.roots).at_least(1).returns(roots);
	}

	void expect_roots(
	        const cppjinja::evtnodes::tmpl* root,
	        std::pmr::vector<const cppjinja::evtnodes::callable*> roots)
	{
		MOCK_EXPECT(env.roots).at_least(1).with(root).returns(roots);
	}
};

} // namespace mocks
