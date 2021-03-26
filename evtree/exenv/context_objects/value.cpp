/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "value.hpp"

cppjinja::east::value_term cppjinja::evt::context_objects::value::tmp_debug_to_val(const json& v) const
{
	using evt = east::value_term;
	if(v.is_string()) return evt{v.get<std::string>()};
	if(v.is_number_integer()) return evt{v.get<std::int64_t>()};
	if(v.is_number_float()) return evt{v.get<double>()};
	if(v.is_boolean()) return evt{v.get<bool>()};
	if(v.is_null()) throw std::runtime_error("cannot convert null to string");
	return evt{v.dump()};
}

cppjinja::evt::context_objects::value::value(cppjinja::json j)
    : jcnt(std::move(j))
{
}

cppjinja::evt::context_objects::value::value(cppjinja::json j, int)
    : value(std::move(j))
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
	return east::value_term(tmp_debug_to_val(jcnt));
}

cppjinja::json cppjinja::evt::context_objects::value::jval() const
{
	return jcnt;
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::value::call(
        std::vector<function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call a value");
}
