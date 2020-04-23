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
#include "obj_holder.hpp"

namespace cppjinja::evt {

class context_impl final : public context {
	struct frame
	{
		const node* maker;
		std::vector<const node*> node_stack;
		std::stringstream out;
		obj_holder ns;
	};
	std::list<frame> ctx;

	void require_not_empty() const ;
public:
	context_impl();
	~context_impl() noexcept ;

	void current_node(const node* n) override ;
	const node* nth_node_on_stack(std::size_t ind) const override ;

	void pop(const node* n)override ;
	void push(const node* n)override ;
	const node* maker() const override ;

	std::ostream& out() override ;
	std::string result() const override ;

	obj_holder& cur_namespace() override ;
	void inject_obj(ast::string_t name, std::unique_ptr<ctx_object> obj) override ;
	void takeout_obj(const ast::string_t& name) override ;

	std::optional<ast::value_term> solve_var(
	        const cppjinja::ast::var_name& var) const override ;
	std::optional<ast::value_term> solve_call(
	        const ast::function_call& call) const override ;
};

} // namespace cppjinja::evt
