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
#include "tree.hpp"

namespace cppjinja::evt::context_objects {

class builtin_function : public context_object {
protected:
	std::shared_ptr<context_object> result_bool(bool res) const ;
	std::shared_ptr<context_object> result_none() const ;
	std::shared_ptr<context_object> result_empty_str() const ;
public:
	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::string_t n) const override ;
	absd::data solve() const override ;
};

class jinja_namespace : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class join : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class builtins : public tree {
	std::shared_ptr<context_object> fnc_upper() const ;
public:
	builtins();
	~builtins() noexcept override ;
};

} // namespace cppjinja::evt::context_objects
