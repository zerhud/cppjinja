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

namespace cppjinja::evt::context_objects {

class delay_call : public context_object {
	std::shared_ptr<context_object> obj;
	std::vector<east::function_parameter> params;
public:
	delay_call(std::shared_ptr<context_object> o,
	           std::vector<east::function_parameter> p);
	~delay_call() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
	json jval() const override;
	std::shared_ptr<context_object> call(std::vector<east::function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
