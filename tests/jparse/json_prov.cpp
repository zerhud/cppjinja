/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "json_prov.hpp"
#include <absd/simple_data_holder.hpp>

cogen::jinja_json_prov::jinja_json_prov(boost::json::value jd)
    : jdata(std::move(jd))
    , mem(std::make_shared<std::pmr::unsynchronized_pool_resource>())
{
}

absd::data cogen::jinja_json_prov::value(const cppjinja::east::var_name& val) const
{
	auto rval = jdata;
	for(auto& n:val) rval = rval.as_object()[n];
	return json_data_holder::create_for_val(mem, std::move(rval));
}

absd::data cogen::jinja_json_prov::value(const cppjinja::east::function_call& val) const
{
	throw std::runtime_error("no such function: " + ref_to_str(val.ref));
}

std::string cogen::jinja_json_prov::ref_to_str(const cppjinja::east::var_name& ref)
{
	std::string ret;
	for(auto& r:ref) ret += std::string(r.begin(),r.end()) + '.';
	if(!ret.empty()) ret.pop_back();
	return ret;
}

cogen::json_data_holder::json_data_holder(boost::json::value val, cogen::storage_ptr s)
    : val(std::move(val))
    , mem(std::move(s))
{
}

std::pmr::memory_resource* cogen::json_data_holder::storage() const
{
	return mem.get();
}

absd::reflection_info cogen::json_data_holder::reflect() const
{
	absd::reflection_info ret{.type=absd::data_type::empty};
	if(val.is_bool()) ret.type = absd::data_type::boolean;
	else if(val.is_double()) ret.type = absd::data_type::floating_point;
	else if(val.is_int64()) ret.type = absd::data_type::integer;
	else if(val.is_string()) ret.type = absd::data_type::string;
	else if(val.is_array()) {
		ret.type = absd::data_type::array;
		ret.size = val.as_array().size();
	}
	else if(val.is_object()) {
		ret.type = absd::data_type::object;
		for(auto& item:val.as_object())
			ret.keys.emplace_back(item.key());
	}
	return ret;
}

std::int64_t cogen::json_data_holder::to_int() const
{
	return val.as_int64();
}

double cogen::json_data_holder::to_double() const
{
	return val.as_double();
}

std::pmr::string cogen::json_data_holder::to_string() const
{
	auto sv = val.as_string();
	return std::pmr::string(sv.begin(),sv.end(), mem.get());
}

bool cogen::json_data_holder::to_bool() const
{
	return val.as_bool();
}

cogen::json_data_holder::self_type cogen::json_data_holder::by_key(std::string_view key) const
{
	return create_for_val(mem, val.as_object().at(key));
}

cogen::json_data_holder::self_type cogen::json_data_holder::by_ind(int64_t ind) const
{
	return create_for_val(mem, val.as_array()[ind]);
}

cogen::json_data_holder::self_type cogen::json_data_holder::create_for_val(
        storage_ptr mem, boost::json::value v)
{
	return std::allocate_shared<json_data_holder>(
	            std::pmr::polymorphic_allocator(mem.get()),
	            std::move(v), mem);
}
