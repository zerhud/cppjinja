/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"
#include "eval/ast_cvt.hpp"

using namespace cppjinja::details;

void cppjinja::evt::context_new::require_not_empty() const
{
	if(ctx.empty()) throw std::runtime_error("context is mepty");
}

std::optional<cppjinja::east::value_term>
cppjinja::evt::context_new::solve_var_name(
        const cppjinja::ast::var_name& var) const
{
	for(auto& ctx_node:ctx.back().node_stack)
		if(auto set = dynamic_cast<const evtnodes::op_set*>(ctx_node);
		        ctx_node->name() == var[0] && set)
			return solve_value(set->value(var));
	return std::nullopt;
}

cppjinja::evt::context_new::context_new()
{
}

void cppjinja::evt::context_new::current_node(const cppjinja::evt::node* n)
{
	require_not_empty();
	ctx.back().node_stack.emplace_back(n);
}

const cppjinja::evt::node*
cppjinja::evt::context_new::current_node(std::size_t ind) const
{
	require_not_empty();
	std::size_t i = ctx.back().node_stack.size() - ind - 1;
	return ctx.back().node_stack.at(i);
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
	ctx.emplace_back(frame{n, {}, {}});
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

std::optional<cppjinja::east::value_term>
cppjinja::evt::context_new::solve_value(
        const cppjinja::ast::value_term& val) const
{
	struct {
		const context_new* self;
		typedef std::optional<east::value_term> optv_t;

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

		optv_t operator()(const double& obj) { return obj; }
		optv_t operator()(const ast::string_t& obj) { return obj; }
		optv_t operator()(const ast::tuple_v& obj) { return make_array(obj.fields); }
		optv_t operator()(const ast::array_v& obj) { return make_array(obj.fields); }
		optv_t operator()(const ast::var_name& obj)
		{
			return self->solve_var_name(obj);
		}
		optv_t operator()(const ast::function_call& obj)
		{
			return 1.1;
		}
		optv_t operator()(const ast::binary_op&) { return false; }
	} rnd{this};

	return boost::apply_visitor(rnd, val.var);
}
