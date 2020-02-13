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

bool cppjinja::evt::node::calculate(const ast::binary_op& op) const
{
	if(op.op == ast::comparator::no)
	{
		// check left is true
		return false;
	}

	struct {
		const ast::value_term& left_val;
		ast::comparator cmp;
	} beval{op.left.get(), op.op} ;

	return false;
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
			auto val = ctx.concreate_value(self, ast::value_term{obj});
			self->render_value(ctx, val);
		}
		void operator()(const ast::function_call& obj) {
			auto* node = ctx.tree().search(obj.ref, self);
			if(!node) {
				auto cv = ctx.concreate_value(self, ast::value_term{obj});
				self->render_value(ctx, cv);
				return;
			}

			ctx.push_callstack(self);
			ctx.call_params(obj.params);
			node->render(ctx);
			ctx.pop_callstack(self);
		}
		void operator()(const ast::binary_op& obj) {
			ctx.out() << self->calculate(obj); }
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
