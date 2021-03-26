/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../context_object.hpp"

namespace cppjinja::evt::context_objects {

class value : public context_object {
	json jcnt;

	east::value_term tmp_debug_to_val(const json& v) const ;
public:
	value(json j);
	value(json j, int);
	~value() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
	json jval() const override;
	std::shared_ptr<context_object> call(std::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
