/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include "../context_object.hpp"
#include "eval/eval.hpp"

namespace cppjinja::evt::context_objects {

class user_data : public context_object {
	const data_provider* provider;
	east::var_name selected_name;
public:
	user_data(user_data&& other) noexcept =default ;
	user_data& operator = (user_data&& other) noexcept =default ;

	user_data(const data_provider* prov);
	user_data(const data_provider* prov, east::var_name selected);
	~user_data() noexcept override ;

	const data_provider* data() const ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	absd::data solve() const override ;
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
