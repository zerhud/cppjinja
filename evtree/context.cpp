/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"
#include "evtree.hpp"
#include "eval/ast_cvt.hpp"
#include "parser/helpers.hpp"

using namespace std::literals;
using namespace cppjinja::details;

const cppjinja::evtnodes::callable* cppjinja::evt::context::cb_ctx_maker() const
{
	return dynamic_cast<const evtnodes::callable*>(ctx_maker());
}

const cppjinja::evtnodes::callable* cppjinja::evt::context::search_in_tree(
        const ast::var_name& n) const
{
	return tree_->search(n, current_node());
}

cppjinja::evt::context::context(
          const data_provider* p
        , const evtree* t
        , std::ostream& out
        )
    : prov(p), tree_(t), outstream(out)
{
}

std::ostream& cppjinja::evt::context::out()
{
	return outstream;
}

const cppjinja::data_provider& cppjinja::evt::context::data() const
{
	return *prov;
}

const cppjinja::evtree& cppjinja::evt::context::tree() const
{
	assert(tree_);
	return *tree_;
}

cppjinja::east::value_term cppjinja::evt::context::concreate_value(
        const cppjinja::ast::value_term& val)
{
	struct {
		context* self;

		east::value_term operator()(const double& obj) { return obj; }
		east::value_term operator()(const ast::string_t& obj) { return obj; }
		east::value_term operator()(const ast::tuple_v&) { return false; }
		east::value_term operator()(const ast::array_v& obj)
		{
			east::array_v ret;
			for(auto&& src:obj.fields)
			{
				auto cv = self->concreate_value(src.get());
				auto val = std::make_unique<east::value_term>(std::move(cv));
				ret.items.emplace_back(std::move(val));
			}

			return ret;
		}
		east::value_term operator()(const ast::var_name& obj)
		{
			std::optional<ast::value_term> par;
			auto* val = self->cb_ctx_maker();
			if(!val || !(par=val->param(*self, obj)))
				val = self->search_in_tree(obj);
			if(val && (par = val->param(*self, obj)))
				return self->concreate_value(*par);
			return self->prov->value(east_cvt::cvt(obj));
		}
		east::value_term operator()(const ast::function_call& obj)
		{
			east::function_call call;
			call.ref = east_cvt::cvt(obj.ref);
			for(auto& src:obj.params)
				call.params.emplace_back(east::function_parameter{
				            src.name,
				            self->concreate_value(src.value.get())});
			return self->prov->value(call);
		}
		east::value_term operator()(const ast::binary_op&) { return false; }
	} rnd{this};

	return boost::apply_visitor(rnd, val.var);
}

cppjinja::east::value_term cppjinja::evt::context::filter(
        const cppjinja::east::value_term& base,
        const cppjinja::ast::value_term& val)
{
	overloaded vget{
		[this, &base](const ast::var_name& obj){
			east::function_call call;
			call.ref = east_cvt::cvt(obj);
			return prov->filter(call, base);
		},
		[this, &base](const ast::function_call& obj){
			east::function_call call;
			call.ref = east_cvt::cvt(obj.ref);
			for(auto& p:obj.params)
				call.params.emplace_back(
				            east::function_parameter{
				                p.name,
				                concreate_value(p.value.get())
				            });
			return prov->filter(call, base);
		},
		[](const auto&) -> east::value_term {
			throw std::logic_error("filter called with unexpected value");
		}
	};

	return boost::apply_visitor(vget, val.var);
}

void cppjinja::evt::context::pop_context(const node* m)
{
	assert( !ctx.empty() );
	assert(ctx.back().maker == m);
	ctx.pop_back();
}

void cppjinja::evt::context::push_context(const node* m)
{
	ctx.push_back({m, {}});
}

void cppjinja::evt::context::add_context(const evt::node* n)
{
	assert( !ctx.empty() );
	ctx.back().ctx.emplace_back(n);
}

void cppjinja::evt::context::current_node(const cppjinja::evt::node* n)
{
	/*
	if(ctx.empty())
	{
		cur_node = n;
		return;
	}

	auto& cur_ctx = ctx.back();
	auto cur_pos = std::find(std::begin(cur_ctx), std::end(cur_ctx), cur_node);
	assert(cur_pos==std::end(cur_ctx) || cur_node!=nullptr);
	if(n!=nullptr)
	{
		auto n_pos = std::find(std::begin(cur_ctx), std::end(cur_ctx), n);
		if( n_pos==std::end(cur_ctx) )
			throw std::runtime_error("cannot find new current node");
		if( ++cur_pos != n_pos )
			throw std::runtime_error("callstack error");
	}
	*/

	cur_nodes.emplace_back(n);
}

const cppjinja::evt::node* cppjinja::evt::context::current_node(std::size_t ind) const
{
	std::size_t i = cur_nodes.size() - ind - 1;
	return cur_nodes.at(i);
}

const cppjinja::evt::node* cppjinja::evt::context::ctx_maker() const
{
	assert( !ctx.empty() );
	return ctx.back().maker;
}

const cppjinja::evt::node* cppjinja::evt::context::caller() const
{
	assert( !callstack.empty() );
	return callstack.back().caller;
}

void cppjinja::evt::context::pop_callstack(const cppjinja::evt::node* n)
{
	assert( !callstack.empty() );
	assert( callstack.back().caller == n );

	callstack.pop_back();
}

void cppjinja::evt::context::push_callstack(const cppjinja::evt::node* n)
{
	callstack.push_back(callstack_frame{n, {}});
}

std::vector<cppjinja::ast::function_call_parameter>
cppjinja::evt::context::call_params() const
{
	assert( !callstack.empty() );
	return callstack.back().params;
}

void cppjinja::evt::context::call_params(
        std::vector<cppjinja::ast::function_call_parameter> params)
{
	assert( !callstack.empty() );
	callstack.back().params = std::move(params);
}
