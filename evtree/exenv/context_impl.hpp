/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <list>

#include "context.hpp"

namespace cppjinja::evt {

class context_impl final : public context {
	struct frame
	{
		const node* maker;
		std::vector<const evtnodes::callable*> injections;
		std::vector<const node*> node_stack;
		std::stringstream out;
		std::unordered_map<ast::string_t, std::function<ast::value_term(ast::var_name)>> vars;
		std::unordered_map<ast::string_t, std::function<ast::value_term(ast::function_call)>> funcs;
	};
	std::list<frame> ctx;

	void require_not_empty() const ;
public:
	context_impl();

	void current_node(const node* n) override ;
	const node* nth_node_on_stack(std::size_t ind) const override ;

	void pop(const node* n)override ;
	void push(const node* n)override ;
	const node* maker() const override ;

	std::ostream& out() override ;
	std::string result() const override ;

	void inject(const evtnodes::callable* n)override ;
	void takeout(const evtnodes::callable* n)override ;
	std::vector<const evtnodes::callable*> injections() const override ;

	std::optional<ast::value_term> solve_var(
	        const cppjinja::ast::var_name& var) const override ;
	std::optional<ast::value_term> solve_call(
	        const ast::function_call& call) const override ;
	void inject_variable(
	          const ast::string_t& n
	        , std::function<ast::value_term(ast::var_name)> g) override ;
	void inject_function(
	          const ast::string_t& n
	        , std::function<ast::value_term(ast::function_call)> g)  override ;
};

} // namespace cppjinja::evt
