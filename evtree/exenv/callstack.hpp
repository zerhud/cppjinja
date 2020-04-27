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
#include "parser/ast/common.hpp"
#include "eval/ast.hpp"

namespace cppjinja::evt {

class callstack {
	struct frame {
		const evtnodes::callable* calling;
		std::vector<ast::function_call_parameter> cparams;
	};
	std::vector<frame> stack;
	void require_stack_is_not_empty() const ;
public:
	virtual ~callstack() noexcept =default ;

	virtual east::string_t call(exenv* env,
	        const evtnodes::callable* calling,
	        std::vector<ast::function_call_parameter> params) =0 ;
	virtual std::vector<const obj_holder*> param_stack(const node* last) const =0 ;

	virtual std::vector<const evtnodes::callable*> calling_stack() const  =0 ;

	virtual std::vector<ast::function_call_parameter> call_params() const  =0 ;

};

} // namespace cppjinja::evt
