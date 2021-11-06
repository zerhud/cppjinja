/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "simple_data_holder.hpp"

#include <cassert>

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
	assert(reflect().type == data_type::empty);
}

std::pmr::memory_resource* simple_dh::storage() const
{
	return mem.get();
}

void simple_dh::set_int(std::int64_t v)
{
	require_change();
	clear_state();
	pod.emplace(v);
}

simple_dh::self_type simple_dh::create_empty() const
{
	auto ret = std::allocate_shared<simple_dh>(alloc_t(storage()), mem);
	assert(ret->reflect().type == data_type::empty);
	return ret;
}

void simple_dh::clear_state()
{
	is_empty_obj = false;
	is_empty_arr = false;
	object.clear();
	array.clear();
}

simple_dh& simple_dh::operator = (double v)
{
	require_change();
	clear_state();
	pod.emplace(v);
	return *this;
}

simple_dh& simple_dh::operator = (bool v)
{
	require_change();
	clear_state();
	pod.emplace(v);
	return *this;
}

void simple_dh::require_change()
{
	is_empty_obj = false;
	is_empty_arr = false;
}

std::pmr::string& simple_dh::str()
{
	require_change();
	clear_state();
	std::pmr::string ret(storage());
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
	make_empty_object();
	auto ret = std::allocate_shared<simple_dh>(alloc_t(storage()), mem);
	auto obj = std::make_pair(std::pmr::string(key, storage()), data{ret});
	object.emplace(std::move(obj));
	return *ret;
}

void simple_dh::put(std::string_view key, data v)
{
	require_change();
	make_empty_object();
	array.clear();
	auto obj = std::make_pair(std::pmr::string(key, storage()), std::move(v));
	object.emplace(std::move(obj));
}

void simple_dh::make_empty_object()
{
	require_change();
	array.clear();
	is_empty_obj = true;
	is_empty_arr = false;
}

simple_dh::self_type simple_dh::by_key(std::string_view key) const
{
	require_extract_obj();
	auto pos = object.find(key);
	if(pos == object.end()) return create_empty();
	assert(dynamic_cast<const simple_dh*>(pos->second.src().get()) != nullptr);
	return pos->second.src();
}

simple_dh::self_type simple_dh::at_key(std::string_view key) const
{
	require_extract_obj();
	auto ret = object.at(std::pmr::string(key,storage()));
	assert(dynamic_cast<const simple_dh*>(ret.src().get()) != nullptr);
	return ret.src();
}

void simple_dh::require_extract_obj() const
{
	if(object.empty())
		throw std::runtime_error("try to extract object from not setted data");
}

simple_dh& simple_dh::push_back()
{
	require_change();
	make_empty_array();
	auto ret = std::allocate_shared<simple_dh>(alloc_t(storage()), mem);
	array.emplace_back(data{ret});
	return *ret;
}

void simple_dh::push_back(absd::data v)
{
	require_change();
	make_empty_array();
	array.emplace_back(std::move(v));
}

void simple_dh::make_empty_array()
{
	require_change();
	object.clear();
	is_empty_obj = false;
	is_empty_arr = true;
}

simple_dh::self_type simple_dh::by_ind(std::int64_t ind) const
{
	require_extract_arr();
	return array.size() <= ind ? create_empty() : array[ind].src();
}

simple_dh::self_type simple_dh::at_ind(std::int64_t ind) const
{
	require_extract_arr();
	return array.at(ind).src();
}

void simple_dh::require_extract_arr() const
{
	if(array.empty())
		throw std::runtime_error("try to extract array from not setted data");
}

absd::reflection_info simple_dh::reflect() const
{
	reflection_info ret{.type=data_type::empty,.size=array.size()};
	if(!object.empty() || is_empty_obj) ret.type = data_type::object;
	else if(!array.empty() || is_empty_arr) ret.type = data_type::array;
	else if(!pod) ret.type = data_type::empty;
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
