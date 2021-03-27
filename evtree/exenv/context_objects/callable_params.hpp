/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <unordered_map>
#include "../context_object.hpp"

namespace cppjinja::evt::context_objects {

class callable_params : public context_object {
	std::vector<east::function_parameter> declared;
	std::unordered_map<std::string, std::shared_ptr<context_object>> params;

	void add_not_passed_defaults();
	void replace_named(function_parameter p);
	void replace_placed(std::size_t place, function_parameter p);
	void add_value(std::string name, absd::data val);
public:
	callable_params(std::vector<east::function_parameter> expected,
	                std::vector<function_parameter> called);
	~callable_params() noexcept override ;

	void add_placed(std::size_t ind, std::shared_ptr<context_object> param);
	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	absd::data solve() const override ;
	std::shared_ptr<context_object> call(
	        std::pmr::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
