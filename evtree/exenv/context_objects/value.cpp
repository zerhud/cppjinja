/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "value.hpp"

cppjinja::evt::context_objects::value::value(cppjinja::east::value_term c)
    : content(std::move(c))
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
cppjinja::evt::context_objects::value::find(cppjinja::east::var_name n) const
{
	(void)n;
	throw std::runtime_error("cannot find a child in a value");
}

cppjinja::east::value_term cppjinja::evt::context_objects::value::solve() const
{
	return content;
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::value::call(
        std::vector<cppjinja::east::function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call a value");
}
