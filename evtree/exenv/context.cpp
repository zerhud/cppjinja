/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"

void cppjinja::evt::context_new::require_not_empty() const
{
	if(ctx.empty()) throw std::runtime_error("context is mepty");
}

cppjinja::evt::context_new::context_new()
{
}

void cppjinja::evt::context_new::current_node(const cppjinja::evt::node* n)
{
	cur_nodes.emplace_back(n);
}

const cppjinja::evt::node*
cppjinja::evt::context_new::current_node(std::size_t ind) const
{
	std::size_t i = cur_nodes.size() - ind - 1;
	return cur_nodes.at(i);
}

void cppjinja::evt::context_new::pop(const cppjinja::evt::node* n)
{
	require_not_empty();
	if(ctx.back().maker != n)
		throw std::runtime_error("cannot pop that wans't pushed");
	ctx.pop_back();
}

void cppjinja::evt::context_new::push(const cppjinja::evt::node* n)
{
	ctx.emplace_back(frame{n, {}});
}

const cppjinja::evt::node* cppjinja::evt::context_new::maker() const
{
	require_not_empty();
	return ctx.back().maker;
}

void cppjinja::evt::context_new::inject(const cppjinja::evtnodes::callable* n)
{
	require_not_empty();
	if(n==nullptr) throw std::runtime_error("cannot inject nullptr");
}

void cppjinja::evt::context_new::takeout(const cppjinja::evtnodes::callable* n)
{
	require_not_empty();
	auto& ins = ctx.back().injections;
	auto pos = std::find(ins.begin(), ins.end(), n);
	if(pos!=ins.end()) ins.erase(pos);
}

cppjinja::east::value_term cppjinja::evt::context_new::solve_value(
        const cppjinja::ast::value_term& val) const
{
	struct {
		const context_new* self;

		east::value_term make_array(
		            const std::vector<ast::forward_ast<ast::value_term>>& fields)
		{
			east::array_v ret;
			for(auto& i:fields)
				ret.items.push_back(
				            std::make_unique<east::value_term>(
				                self->solve_value(i.get())));
			return ret;
		}

		east::value_term operator()(const double& obj) { return obj; }
		east::value_term operator()(const ast::string_t& obj) { return obj; }
		east::value_term operator()(const ast::tuple_v& obj) { return make_array(obj.fields); }
		east::value_term operator()(const ast::array_v& obj) { return make_array(obj.fields); }
		east::value_term operator()(const ast::var_name& obj)
		{
			return 1.1;
		}
		east::value_term operator()(const ast::function_call& obj)
		{
			return 1.1;
		}
		east::value_term operator()(const ast::binary_op&) { return false; }
	} rnd{this};

	return boost::apply_visitor(rnd, val.var);
}
