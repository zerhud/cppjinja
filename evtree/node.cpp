/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "node.hpp"
#include "parser/helpers.hpp"

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
		void operator()(const ast::var_name& obj) { ctx.render_variable(obj); }
		void operator()(const ast::function_call& obj) {
			ctx.push_callstack(self, false);
			ctx.call_params(obj.params);
			ctx.render_function(obj);
			ctx.pop_callstack(self);
		}
		void operator()(const ast::binary_op&) { ctx.out() << "binary"; }
	};

	renderer rnd(ctx, this);
	boost::apply_visitor(rnd, value.var);
}
