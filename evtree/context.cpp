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

const cppjinja::evt::node* cppjinja::evt::context::search_in_tree_by_ctx(
        const ast::var_name& n) const
{
	if(n.size() != 1) return nullptr;

	auto children = tree_->children(ctx_maker(), false);
	for(auto& child:children) if(child->name() == n[0]) return child;

	return nullptr;
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::context::search_in_params(const cppjinja::ast::var_name& obj) const
{
	using evtnodes::callable;
	for(auto i:ctx.back().injections)
		if(auto p = i->param(*this, obj);p)
			return p;
	if(const callable* v = cb_ctx_maker())
		return v->param(*this, obj);
	return std::nullopt;
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
			std::optional<ast::value_term> result = self->search_in_params(obj);
			if(result) return self->concreate_value(*result);

			auto* set = dynamic_cast<const evtnodes::op_set*>(
			            self->search_in_tree_by_ctx(obj));
			return set
			        ? self->concreate_value(set->value(obj))
			        : self->prov->value(east_cvt::cvt(obj))
			        ;
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
	ctx.push_back({m, {}, {}});
}

void cppjinja::evt::context::add_to_context(const evt::node* n)
{
	assert( !ctx.empty() );
	ctx.back().ctx.emplace_back(n);
}

void cppjinja::evt::context::inject_to_context(const cppjinja::evtnodes::callable* node)
{
	ctx.back().injections.emplace_back(node);
}

void cppjinja::evt::context::takeout_from_context(const cppjinja::evtnodes::callable* node)
{
	auto& ins = ctx.back().injections;
	auto pos = std::find(ins.begin(), ins.end(), node);
	if(pos!=ins.end()) ins.erase(pos);
}

void cppjinja::evt::context::current_node(const cppjinja::evt::node* n)
{
	cur_nodes.emplace_back(n);
}

const cppjinja::evt::node*
cppjinja::evt::context::current_node(std::size_t ind) const
{
	std::size_t i = cur_nodes.size() - ind - 1;
	return cur_nodes.at(i);
}

std::optional<cppjinja::evt::render_info>
cppjinja::evt::context::cur_render_info() const
{
	return cur_info;
}

void cppjinja::evt::context::cur_render_info(std::optional<render_info> ni)
{
	cur_info = ni;
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

cppjinja::evt::maker_context::maker_context(
        const cppjinja::evt::node* n, cppjinja::evt::context* c)
    : ctx(c), maker(n)
{
	if(ctx && maker) ctx->push_context(maker);
}

cppjinja::evt::maker_context::~maker_context()
{
	if(ctx && maker) ctx->pop_context(maker);
}

cppjinja::evt::maker_callstack::maker_callstack(
        const cppjinja::evt::node* n, cppjinja::evt::context* c)
    : ctx(c), maker(n)
{
	if(ctx && maker) ctx->push_callstack(maker);
}

cppjinja::evt::maker_callstack::~maker_callstack()
{
	if(ctx && maker) ctx->pop_callstack(maker);
}

cppjinja::evt::maker_injection::maker_injection(
        const cppjinja::evtnodes::callable* i, cppjinja::evt::context* c)
    : ctx(c), injection(i)
{
	if(ctx && injection) ctx->inject_to_context(injection);
}

cppjinja::evt::maker_injection::~maker_injection()
{
	if(ctx && injection) ctx->takeout_from_context(injection);
}
