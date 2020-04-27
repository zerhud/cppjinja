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

namespace cppjinja::evt {

class callstack_impl final : public callstack {
	struct frame {
		const evtnodes::callable* calling;
		std::vector<ast::function_call_parameter> cparams;
		obj_holder params;
	};
	std::vector<frame> stack;
	void require_stack_is_not_empty() const ;
	std::optional<ast::value_term> position_param(
	        const std::vector<ast::function_call_parameter>& params,
	        std::size_t ind) const ;

	void pop() ;
	void push(const evtnodes::callable* calling_stack) ;
	void call_params(std::vector<ast::function_call_parameter> params) ;
	void make_params_holder(std::vector<ast::function_call_parameter> params);
public:
	east::string_t call(exenv* env,
	        const evtnodes::callable* calling,
	        std::vector<ast::function_call_parameter> params) override ;
	std::vector<const obj_holder*> param_stack(const node* last) const override ;
};

} // namespace cppjinja::evt
