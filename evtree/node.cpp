/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "node.hpp"
#include "parser/helpers.hpp"
#include "evtree.hpp"
#include "eval/ast.hpp"
#include "helpers/binary_op.hpp"
#include "helpers/loop_by_three.hpp"

using namespace std::literals;

void cppjinja::evt::node::add_parent(cppjinja::evt::node* np)
{
	assert(
	      std::find(
	                std::begin(parents_),
	                std::end(parents_),
	                np) == std::end(parents_)
	      );

	parents_.emplace_back(np);
}

std::vector<cppjinja::evt::node*> cppjinja::evt::node::parents()
{
	return parents_;
}

std::vector<const cppjinja::evt::node*> cppjinja::evt::node::parents() const
{
	std::vector<const cppjinja::evt::node*> ret;
	for(auto& p:parents_) ret.emplace_back(p);
	return ret;
}

bool cppjinja::evt::node::is_parent(const cppjinja::evt::node* n) const
{
	for(auto& p:parents_) if(p==n) return true;
	return false;
}

void cppjinja::evt::node::render_children(
        const std::vector<const node*>& children,
        context& ctx, render_info default_ri) const
{
	for(auto&& child:children) ctx.add_to_context(child);
	auto rnd = [&ctx,&default_ri]
	    (const node* p, const node* c, const node* n)
	{
		render_info ri = default_ri;
		if(p) ri.trim_left = p->rinfo().trim_right;
		if(n) ri.trim_right = n->rinfo().trim_left;
		ctx.cur_render_info(std::move(ri));
		c->render(ctx);
	};
	evtnodes::loop_by_three(children, rnd);
}

bool cppjinja::evt::node::calculate(
        context& ctx, const ast::binary_op& op) const
{
	evtnodes::binary_op_helper cmp(op.op);
	east::value_term left = ctx.concreate_value(op.left);
	east::value_term right = ctx.concreate_value(op.right);
	return std::visit(cmp,
	            (east::value_term_var&)left,
	            (east::value_term_var&)right);
}

void cppjinja::evt::node::render_value(
          context& ctx
        , const cppjinja::ast::value_term& value
        ) const
{
	struct renderer {
		context& ctx;
		const node* self;
		renderer(context& c, const node* s) : ctx(c), self(s) {}

		void operator()(const double& obj) { ctx.out() << obj; }
		void operator()(const ast::string_t& obj) { ctx.out() << obj; }
		void operator()(const ast::tuple_v&) { }
		void operator()(const ast::array_v&) { }
		void operator()(const ast::var_name& obj) {
			auto val = ctx.concreate_value(ast::value_term{obj});
			self->render_value(ctx, val);
		}
		void operator()(const ast::function_call& obj) {
			auto* node = ctx.tree().search(obj.ref, self);
			if(!node) {
				auto cv = ctx.concreate_value(ast::value_term{obj});
				self->render_value(ctx, cv);
				return;
			}

			maker_callstack cmaker(self, &ctx);
			ctx.call_params(obj.params);
			node->render(ctx);
		}
		void operator()(const ast::binary_op& obj) {
			ctx.out() << self->calculate(ctx, obj); }
	};

	renderer rnd(ctx, this);
	boost::apply_visitor(rnd, value.var);
}


void cppjinja::evt::node::render_value(
          context& ctx
        , const cppjinja::east::value_term& value
        ) const
{
	struct {
		context& ctx;
		void operator()(const double& v) { ctx.out() << v ; }
		void operator()(const east::string_t& v) { ctx.out() << v; }
		void operator()(const east::array_v&) {}
		void operator()(const east::map_v&) {}
	} rnd{ctx};

	const east::value_term_var& var_val = value;
	std::visit(rnd, var_val);
}
