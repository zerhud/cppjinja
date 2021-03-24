/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "abstract_data.hpp"

#include <cassert>
#include <iomanip>

using abs_data = absd::data;

namespace absd {
template<class... T> struct overloaded : T... { using T::operator()...; };
template<class... T> overloaded(T...) -> overloaded<T...>;
std::ostream& pbool(std::ostream& out, bool v)
{
	return out << (v?"true":"false");
}
} // namespace absd

std::ostream& absd::operator << (std::ostream& out, const data& src)
{
	auto info = src.source->reflect();
	if(src.is_pod(info)) {
		if(src.is_cached()) {
			std::visit( overloaded {
			         [&out](auto&& d){out << d;},
			         [&out](bool d){pbool(out, d);},
			         [&out](std::pmr::string& d){out << std::quoted(d);}
			    }, src.cache.value());
		} else if(info.type == data_type::integer) out << (std::int64_t)src;
		else if(info.type == data_type::floating_point) out << (double)src;
		else if(info.type == data_type::string) out << std::quoted(src.str());
		else if(info.type == data_type::boolean) pbool(out, (bool)src);
		else {
			assert(false);
			throw std::logic_error("not all data types cached");
		}
	} else if(info.type == data_type::array) {
		out << '[';
		if(!info.size == 0) out << src[0];
		for(std::uint64_t i=1;i<info.size;++i)
			out << ',' << src[i];
		out << ']';
	} else if(info.type == data_type::object) {
		out << '{';
		if(!info.keys.empty()) {
			auto last = info.keys.back();
			info.keys.pop_back();
			for(auto& k:info.keys)
				out << std::quoted(k) << ':' << src[k] << ',';
			out << std::quoted(last) << ':' << src[last] ;
		}
		out << '}';
	} else {
		assert(false);
		throw std::logic_error("not all data types cached");
	}
	return out;
}

abs_data::data(std::shared_ptr<absd::data_holder> src)
    : source(std::move(src))
{
}

bool abs_data::is_cached() const
{
	return
	        cache.has_value()
	     || !key_cache.empty()
	     || !ind_cache.empty()
	        ;
}

bool abs_data::is_pod(const reflection_info& info)
{
	data_type t = info.type;
	return t!=data_type::array && t!=data_type::object;
}

abs_data::operator std::int64_t() const
{
	assert(source);
	if(!is_cached()) cache = source->to_int();
	return std::get<std::int64_t>(cache.value());
}

abs_data::operator double() const
{
	assert(source);
	if(!is_cached()) cache = source->to_double();
	return std::get<double>(cache.value());
}

abs_data::operator std::pmr::string() const
{
	assert(source);
	if(!is_cached()) cache = source->to_string();
	return std::get<std::pmr::string>(cache.value());
}

abs_data::operator bool() const
{
	assert(source);
	if(!is_cached()) cache = source->to_bool();
	return std::get<bool>(cache.value());
}

const abs_data& abs_data::operator [](std::string_view key) const
{
	std::string skey(key);
	if(is_cached() && key_cache.contains(key))
		return key_cache.at(skey);
	key_cache.emplace(std::make_pair(skey, abs_data(source->by_key(key))));
	return key_cache.at(skey);
}

const abs_data& abs_data::operator [](std::int64_t ind) const
{
	if(is_cached() && ind_cache.contains(ind))
		return ind_cache.at(ind);
	ind_cache.emplace(std::make_pair(ind, abs_data(source->by_ind(ind))));
	return ind_cache.at(ind);
}
