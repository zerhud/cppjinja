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
		const node* caller;
		const evtnodes::callable* calling;
		std::vector<ast::function_call_parameter> cparams;
	};
	std::vector<frame> stack;
	void require_stack_is_not_empty() const ;
public:
	void pop(const node* caller);
	void push(const node* caller, const evtnodes::callable* calling_stack);
	const node* caller() const ;
	std::vector<const evtnodes::callable*> calling_stack() const ;
	std::vector<ast::function_call_parameter> call_params() const ;
	void call_params(std::vector<ast::function_call_parameter> params);

};

} // namespace cppjinja::evt
