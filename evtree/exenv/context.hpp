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

namespace cppjinja::evt {

class context {
public:
	virtual ~context() noexcept =default ;

	virtual void current_node(const node* n) =0 ;
	virtual const node* nth_node_on_stack(std::size_t ind) const =0 ;

	virtual void pop(const node* n)=0 ;
	virtual void push(const node* n)=0 ;
	virtual const node* maker() const =0 ;

	virtual std::ostream& out() =0 ;
	virtual std::string result() const =0 ;

	virtual void inject_obj(ast::string_t name, std::unique_ptr<ctx_object> obj) =0 ;
	virtual void takeout_obj(const ast::string_t& name) =0 ;

	virtual std::optional<ast::value_term> solve_var(
	        const ast::var_name& var) const=0 ;
	virtual std::optional<ast::value_term> solve_call(
	        const ast::function_call& call) const=0 ;
};

} // namespace cppjinja::evt
