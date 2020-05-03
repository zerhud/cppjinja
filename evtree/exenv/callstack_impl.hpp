/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "callstack.hpp"
#include "obj_holder.hpp"
#include "context_objects/callable_params.hpp"

namespace cppjinja::evt {

class callstack_impl final : public callstack {
	struct frame {
		const evtnodes::callable* calling;
		obj_holder params;
		std::optional<context_objects::callable_params> param_object;
	};
	std::vector<frame> stack;

	std::optional<ast::value_term> position_param(
	        const std::vector<ast::function_call_parameter>& params,
	        std::size_t ind) const ;

	void make_params_holder(std::vector<ast::function_call_parameter> params);
	void make_params_holder(std::vector<east::function_parameter> params);
public:
	east::string_t call(exenv* env,
	        const evtnodes::callable* calling,
	        std::vector<ast::function_call_parameter> params) override ;
	std::vector<const obj_holder*> param_stack(const node* last) const override ;

	context_objects::queue current_params(const node* last) const override ;
	void pop() override ;
	void push(
	          const evtnodes::callable* calling
	        , context_objects::callable_params params) override ;
};

} // namespace cppjinja::evt
