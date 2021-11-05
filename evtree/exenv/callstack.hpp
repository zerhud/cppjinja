/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "evtree/declarations.hpp"
#include "context_objects/queue.hpp"
#include "context_objects/callable_params.hpp"
#include "eval/ast.hpp"

namespace cppjinja::evt {

class callstack {
public:
	virtual ~callstack() noexcept =default ;

	virtual const context_objects::queue current_params(const node* last) const =0 ;
	virtual void pop() =0 ;
	virtual void push(
	          const evtnodes::callable* calling
	        , context_objects::callable_params params) =0 ;

};

} // namespace cppjinja::evt
