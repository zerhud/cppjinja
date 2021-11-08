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

class value :
        public context_object
      , public std::enable_shared_from_this<value>
{
	absd::data src;

public:
	value(absd::data j);
	~value() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::string_t n) const override ;
	absd::data solve() const override ;
	std::shared_ptr<context_object> call(
	        std::pmr::vector<function_parameter> params) const override ;
};

template<typename T>
std::shared_ptr<value> make_val(T&& v)
{
	return std::make_shared<value>(std::forward<T>(v));
}

} // namespace cppjinja::evt::context_objects
