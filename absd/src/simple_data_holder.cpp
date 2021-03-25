/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "simple_data_holder.hpp"

using abs_data = absd::data;
using simple_dh = absd::simple_data_holder;

simple_dh::simple_data_holder()
    : simple_data_holder(std::make_shared<std::pmr::unsynchronized_pool_resource>())
{
}

simple_dh::simple_data_holder(std::shared_ptr<std::pmr::memory_resource> m)
    : mem(std::move(m))
    , object(mem.get())
    , array(mem.get())
{
}

std::pmr::memory_resource& simple_dh::storage() const
{
	return *mem;
}

void simple_dh::set_int(std::int64_t v)
{
	require_change();
	pod.emplace(v);
}

simple_dh& simple_dh::operator = (double v)
{
	require_change();
	pod.emplace(v);
	return *this;
}

simple_dh& simple_dh::operator = (bool v)
{
	require_change();
	pod.emplace(v);
	return *this;
}

void simple_dh::require_change() const
{
	const bool is_setted =
	    pod.has_value() || !object.empty() || !array.empty();
	if(is_setted)
		throw std::runtime_error("double set");
}

std::pmr::string& simple_dh::str()
{
	require_change();
	std::pmr::string ret(&storage());
	pod.emplace(std::move(ret));
	return std::get<std::pmr::string>(*pod);
}

const std::pmr::string& simple_dh::str() const
{
	require_extract_pod();
	return std::get<std::pmr::string>(*pod);
}

void simple_dh::require_extract_pod() const
{
	if(!pod.has_value())
		throw std::runtime_error("try to extract not setted value");
}

simple_dh& simple_dh::put(std::string_view key)
{
	require_change();
	auto ret = std::allocate_shared<simple_dh>(alloc_t(&storage()), mem);
	object[std::pmr::string(key, &storage())] = ret;
	return *ret;
}

simple_dh::self_type simple_dh::by_key(std::string_view key) const
{
	require_extract_obj();
	return object.at(std::pmr::string(key,&storage()));
}

void simple_dh::require_extract_obj() const
{
	if(object.empty())
		throw std::runtime_error("try to extract object from not setted data");
}

simple_dh& simple_dh::push_back()
{
	require_change();
	auto ret = std::allocate_shared<simple_dh>(alloc_t(&storage()), mem);
	array.emplace_back(ret);
	return *ret;
}

simple_dh::self_type simple_dh::by_ind(std::int64_t ind) const
{
	require_extract_arr();
	return array.at(ind);
}

void simple_dh::require_extract_arr() const
{
	if(array.empty())
		throw std::runtime_error("try to extract array from not setted data");
}

absd::reflection_info simple_dh::reflect() const
{
	reflection_info ret{.type=data_type::empty,.size=array.size()};
	if(!object.empty()) ret.type = data_type::object;
	else if(!array.empty()) ret.type = data_type::array;
	else if(std::holds_alternative<std::int64_t>(*pod))
		ret.type = data_type::integer;
	else if(std::holds_alternative<double>(*pod))
		ret.type = data_type::floating_point;
	else if(std::holds_alternative<bool>(*pod))
		ret.type = data_type::boolean;
	else if(std::holds_alternative<std::pmr::string>(*pod))
		ret.type = data_type::string;
	for(auto& [k,v]:object) ret.keys.emplace_back(k);
	return ret;
}

std::int64_t simple_dh::to_int() const
{
	require_extract_pod();
	return std::get<std::int64_t>(*pod);
}

double simple_dh::to_double() const
{
	require_extract_pod();
	return std::get<double>(*pod);
}

std::pmr::string simple_dh::to_string() const
{
	require_extract_pod();
	return std::get<std::pmr::string>(*pod);
}

bool simple_dh::to_bool() const
{
	require_extract_pod();
	return std::get<bool>(*pod);
}
