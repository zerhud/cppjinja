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
#include "context_objects/callable_params.hpp"

namespace cppjinja::evt {

class callstack_impl final : public callstack {
	struct frame {
		const evtnodes::callable* calling;
		std::optional<context_objects::callable_params> param_object;
	};
	std::vector<frame> stack;
public:
	context_objects::queue current_params(const node* last) const override ;
	void pop() override ;
	void push(
	          const evtnodes::callable* calling
	        , context_objects::callable_params params) override ;
};

} // namespace cppjinja::evt
