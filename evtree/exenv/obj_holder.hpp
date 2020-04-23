/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <unordered_map>
#include "evtree/declarations.hpp"
#include "parser/ast/common.hpp"

namespace cppjinja::evt {

class obj_holder final {
	std::unordered_map<ast::string_t, std::unique_ptr<ctx_object>> map;
public:
	obj_holder(obj_holder&& other) =default ;
	obj_holder() =default ;
	~obj_holder() noexcept ;

	void add(ast::string_t n, std::unique_ptr<ctx_object> o);
	void rem(const ast::string_t& n);
	std::optional<ast::value_term> call(ast::function_call fnc) const ;
	std::optional<ast::value_term> solve(ast::var_name var) const ;
};

} // namespace cppjinja::evt
