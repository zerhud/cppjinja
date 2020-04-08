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

std::vector<cppjinja::evt::node*> cppjinja::evt::node::parents(bool all)
{
	auto ret = parents_;
	if(all) for(auto&p:ret) {
		auto ppars = p->parents(true);
		ret.insert(ret.end(), ppars.begin(), ppars.end());
	}
	return ret;
}

std::vector<const cppjinja::evt::node*> cppjinja::evt::node::parents(bool all) const
{
	std::vector<const cppjinja::evt::node*> ret;
	for(auto& p:parents_) ret.emplace_back(p);
	if(all) for(auto&p:ret) {
		auto ppars = p->parents(true);
		ret.insert(ret.end(), ppars.begin(), ppars.end());
	}
	return ret;
}

bool cppjinja::evt::node::is_parent(const cppjinja::evt::node* n) const
{
	for(auto& p:parents_) if(p==n) return true;
	return false;
}

void cppjinja::evt::node::render_children(
        const std::vector<const node*>& children,
        exenv& ctx, render_info default_ri) const
{
	auto rnd = [&ctx,&default_ri]
	    (const node* p, const node* c, const node* n)
	{
		render_info ri = default_ri;
		if(p) ri.trim_left = p->rinfo().trim_right;
		if(n) ri.trim_right = n->rinfo().trim_left;
		ctx.rinfo(std::move(ri));
		c->render(ctx);
	};
	evtnodes::loop_by_three(children, rnd);
}

bool cppjinja::evt::node::calculate(
        exenv& ctx, const ast::binary_op& op) const
{
	//TODO: remove this method (exenv already do it)?
	evtnodes::binary_op_helper cmp(op.op);
	east::value_term left = ctx.solve_value(op.left);
	east::value_term right = ctx.solve_value(op.right);
	return std::visit(cmp,
	            (east::value_term_var&)left,
	            (east::value_term_var&)right);
}

void cppjinja::evt::node::render_value(
          exenv& ctx
        , const cppjinja::ast::value_term& value
        ) const
{
	struct renderer {
		exenv& ctx;
		const node* self;
		renderer(exenv& c, const node* s) : ctx(c), self(s) {}

		void operator()(const double& obj) { ctx.out() << obj; }
		void operator()(const ast::string_t& obj) { ctx.out() << obj; }
		void operator()(const ast::tuple_v&) { }
		void operator()(const ast::array_v&) { }
		void operator()(const ast::var_name& obj) {
			auto val = ctx.solve_value(ast::value_term{obj});
			self->render_value(ctx, val);
		}
		void operator()(const ast::function_call& obj) {
			auto* node = ctx.tmpl().search(obj.ref, self);
			if(!node) {
				auto cv = ctx.solve_value(ast::value_term{obj});
				self->render_value(ctx, cv);
				return;
			}

			auto* canode = dynamic_cast<const evtnodes::callable*>(node);
			if(!canode) throw std::runtime_error("cannot call not callable node");
			raii_push_callstack cmaker(self, canode, &ctx.calls());
			ctx.calls().call_params(obj.params);
			node->render(ctx);
		}
		void operator()(const ast::binary_op& obj) {
			ctx.out() << self->calculate(ctx, obj); }
	};

	renderer rnd(ctx, this);
	boost::apply_visitor(rnd, value.var);
}


void cppjinja::evt::node::render_value(
          exenv& ctx
        , const cppjinja::east::value_term& value
        ) const
{
	struct {
		exenv& ctx;
		void operator()(const double& v) { ctx.out() << v ; }
		void operator()(const east::string_t& v) { ctx.out() << v; }
		void operator()(const east::array_v&) {}
		void operator()(const east::map_v&) {}
	} rnd{ctx};

	const east::value_term_var& var_val = value;
	std::visit(rnd, var_val);
}
