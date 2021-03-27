/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include "evtree/declarations.hpp"
#include "eval/eval.hpp"
#include "eval/ast.hpp"

namespace cppjinja::evt {

class context_object {
public:
	struct function_parameter {
		std::optional<east::string_t> name;
		std::shared_ptr<context_object> value;
	};

	virtual ~context_object() noexcept =default ;
	virtual void add(east::string_t n, std::shared_ptr<context_object> child) =0 ;
	virtual std::shared_ptr<context_object> find(east::var_name n) const =0 ;
	virtual absd::data solve() const =0 ;
	virtual std::shared_ptr<context_object> call(
	        std::pmr::vector<function_parameter> params) const =0 ;
};

inline bool operator == (
        const context_object::function_parameter& l,
        const context_object::function_parameter& r)
{
	return
	           l.name.has_value() == r.name.has_value()
	        && l.name.value_or("") == r.name.value_or("")
	        && l.value->solve() == r.value->solve()
	        ;
}

} // namespace cppjinja::evt
