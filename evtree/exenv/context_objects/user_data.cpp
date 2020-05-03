/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "user_data.hpp"

using namespace std::literals;

cppjinja::evt::context_objects::user_data::user_data(std::shared_ptr<cppjinja::data_provider> prov)
    : user_data(std::move(prov), east::var_name{})
{
}

cppjinja::evt::context_objects::user_data::user_data(
          std::shared_ptr<cppjinja::data_provider> prov
        , cppjinja::east::var_name selected)
    : provider(std::move(prov))
    , selected_name(std::move(selected))
{
}

cppjinja::evt::context_objects::user_data::~user_data() noexcept
{
}

void cppjinja::evt::context_objects::user_data::add(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add something to user data");
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::user_data::find(cppjinja::east::var_name n) const
{
	return std::make_shared<user_data>(provider, std::move(n));
}

cppjinja::east::value_term cppjinja::evt::context_objects::user_data::solve() const
{
	return provider->value(selected_name);
}

cppjinja::east::value_term cppjinja::evt::context_objects::user_data::call(std::vector<cppjinja::east::function_parameter> params) const
{
	east::function_call call;
	call.ref = selected_name;
	call.params = std::move(params);
	return provider->value(call);
}
