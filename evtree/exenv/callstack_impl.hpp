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

namespace cppjinja::evt {

class callstack_impl final : public callstack {
	struct frame {
		const evtnodes::callable* calling;
		std::vector<ast::function_call_parameter> cparams;
	};
	std::vector<frame> stack;
	void require_stack_is_not_empty() const ;
	std::optional<ast::value_term> position_param(
	        const std::vector<ast::function_call_parameter>& params,
	        std::size_t ind) const ;
public:
	void pop() override ;
	void push(const evtnodes::callable* calling_stack) override ;
	std::vector<const evtnodes::callable*> calling_stack() const  override ;
	std::shared_ptr<evtnodes::callable_multisolver> make_params(exenv* env,
	        const std::vector<ast::function_call_parameter>& params) const override ;
	std::vector<ast::function_call_parameter> call_params() const  override ;
	void call_params(std::vector<ast::function_call_parameter> params) override ;

};

} // namespace cppjinja::evt
