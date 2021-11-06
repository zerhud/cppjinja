/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "callable_params.hpp"

#include <cassert>
#include <algorithm>

#include "value.hpp"

using namespace std::literals;
using cppjinja::evt::context_objects::callable_params;

void callable_params::add_not_passed_defaults()
{
	for(east::function_parameter& p:declared) {
		assert(p.name.has_value());
		if(p.val.has_value()) add_value(*p.name, *p.val);
	}
}

void callable_params::replace_named(function_parameter p)
{
	assert(p.value);
	assert(p.name.has_value());
	params[*p.name] = std::move(p.value);
	auto pos = std::find_if(declared.begin(),declared.end(),
	                        [&p](auto& d){return p.name == d.name;});
	if(pos!=declared.end()) pos->val.reset();
}

void callable_params::replace_placed(std::size_t place, function_parameter p)
{
	assert(p.value);
	assert(declared.at(place).name.has_value());
	auto& name = *declared[place].name;
	params[name] = std::move(p.value);
	declared[place].val.reset();
}

void callable_params::add_value(std::pmr::string name, absd::data val)
{
	params[name] = std::make_shared<value>(std::move(val));
}

callable_params::callable_params(
          std::pmr::vector<cppjinja::east::function_parameter> expected
        , std::pmr::vector<function_parameter> called
        )
    : declared(std::move(expected))
{
	for(std::size_t i=0;i<called.size();++i) {
		if(called[i].name.has_value()) replace_named(std::move(called[i]));
		else replace_placed(i, std::move(called[i]));
	}
	add_not_passed_defaults();
}

callable_params::~callable_params() noexcept
{
}

void callable_params::add_placed(
        std::size_t ind, std::shared_ptr<cppjinja::evt::context_object> param)
{
	auto name = declared.at(ind).name;
	assert(name);
	params[*name] = std::move(param);
}

void callable_params::add(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	if(!child) throw std::runtime_error("cannot add nullptr as a child");
	params[n] = std::move(child);
}

std::shared_ptr<cppjinja::evt::context_object>
callable_params::find(cppjinja::east::string_t n) const
{
	auto pos = params.find(n);
	return pos==params.end() ? nullptr : pos->second;
}

absd::data callable_params::solve() const
{
	throw std::runtime_error("cannot cannot solve a callable_params");
}

std::shared_ptr<cppjinja::evt::context_object> callable_params::call(
        std::pmr::vector<function_parameter>) const
{
	throw std::runtime_error("cannot cannot call a callable_params");
}
