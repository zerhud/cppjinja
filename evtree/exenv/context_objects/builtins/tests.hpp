/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../builtins.hpp"

namespace cppjinja::evt::context_objects::builtins_tests {

class callable : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class defined : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class undefined : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class sameas : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class upper : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class lower : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

class iterable : public builtin_function {
public:
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects::builtins_objs
