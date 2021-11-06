/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "value.hpp"

using namespace std::literals;

cppjinja::evt::context_objects::value::value(absd::data j)
    : src(std::move(j))
{
}

cppjinja::evt::context_objects::value::~value() noexcept
{
}

void cppjinja::evt::context_objects::value::add(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add a child to a value");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::value::find(cppjinja::east::string_t n) const
{
	if(src.is_object()) return std::make_shared<value>(src[n]);
	if(src.is_array()) return std::make_shared<value>(src[std::stoi(n.c_str())]);
	throw std::runtime_error("cannot find a child in a value ["s + n.c_str() + ']');
}

absd::data cppjinja::evt::context_objects::value::solve() const
{
	return src;
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::value::call(
        std::pmr::vector<function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call a value");
}
