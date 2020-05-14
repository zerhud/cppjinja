/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "delay_call.hpp"
#include <algorithm>

cppjinja::evt::context_objects::delay_call::delay_call(
          std::shared_ptr<cppjinja::evt::context_object> o
        , std::vector<cppjinja::east::function_parameter> p)
    : obj(std::move(o))
    , params(std::move(p))
{
}

cppjinja::evt::context_objects::delay_call::~delay_call() noexcept
{
}

void cppjinja::evt::context_objects::delay_call::add(
        cppjinja::east::string_t, std::shared_ptr<cppjinja::evt::context_object>)
{
	throw std::runtime_error("cannot add child to delay_call");
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::delay_call::find(east::var_name) const
{
	throw std::runtime_error("cannot find a child in a delay_call");
}

cppjinja::east::value_term cppjinja::evt::context_objects::delay_call::solve() const
{
	return obj->call(params)->solve();
}

cppjinja::json cppjinja::evt::context_objects::delay_call::jval() const
{
	return obj->call(params)->jval();
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::delay_call::call(
        std::vector<cppjinja::east::function_parameter> params) const
{
	std::vector<cppjinja::east::function_parameter> result = this->params;
	auto ins = std::back_inserter(result);
	for(auto& ip:params) {
		if(ip.name) {
			auto check_name = [&ip](auto p){return p.name == ip.name;};
			auto pos = std::find_if(result.begin(),result.end(),check_name);
			if(pos == result.end()) ins = ip;
			else pos->jval = ip.jval;
		}
	}
	return obj->call(result);
}
