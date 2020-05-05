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

class builtin_function : public context_object {
public:
	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
};

class jinja_namespace : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::vector<east::function_parameter> params) const override ;
};

class builtins : public context_object {
	std::unordered_map<std::string, std::shared_ptr<context_object>> functions;
public:
	builtins();
	~builtins() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
	std::shared_ptr<context_object> call(std::vector<east::function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
