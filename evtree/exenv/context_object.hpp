/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include "eval/ast.hpp"

namespace cppjinja::evt {

class context_object {
public:
	virtual ~context_object() noexcept =default ;
	virtual void add(east::string_t n, std::shared_ptr<context_object> child) =0 ;
	virtual std::shared_ptr<context_object> find(east::var_name n) const =0 ;
	virtual east::value_term solve() const =0 ;
	virtual std::shared_ptr<context_object> call(std::vector<east::function_parameter> params) const =0 ;
};

} // namespace cppjinja::evt
