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
using to_json = absd::to_json_printer;

namespace absd {
template<class... T> struct overloaded : T... { using T::operator()...; };
template<class... T> overloaded(T...) -> overloaded<T...>;
} // namespace absd

to_json::to_json_printer(std::ostream &out) : out(out) {}
std::ostream& to_json::operator () (const data& to_print)
{
	src = &to_print;
	info = src->source->reflect();
	if(src->is_pod(info)) return src->is_cached() ? cached() : pod();
	if(info.type == data_type::array) return array();
	if(info.type == data_type::object) return object();
	if(info.type == data_type::empty) throw std::runtime_error("try to print none");
	assert(false);
	throw std::logic_error("not all data types cached");
}
std::ostream& to_json::cached()
{
	std::visit( overloaded {
	     [this](auto&& d){out << d;},
	     [this](bool){pbool();},
	     [this](std::pmr::string& d){out << std::quoted(d);}
	    }, src->cache.value());
	return out;
}
std::ostream& to_json::pod()
{
	if(info.type == data_type::integer) return out << (std::int64_t)*src;
	if(info.type == data_type::floating_point) return out << (double)*src;
	if(info.type == data_type::string) return out << std::quoted(src->str());
	if(info.type == data_type::boolean) return pbool();
	assert(false);
	throw std::logic_error("not all data types cached");
}
std::ostream& to_json::pbool()
{
	return out << (*src?"true":"false");
}
std::ostream& to_json::array()
{
	out << '[';
	if(!info.size == 0) out << (*src)[0];
	for(std::uint64_t i=1;i<info.size;++i)
		out << ',' << (*src)[i];
	return out << ']';
}
std::ostream& to_json::object()
{
	out << '{';
	if(!info.keys.empty()) {
		auto last = info.keys.back();
		info.keys.pop_back();
		for(auto& k:info.keys)
			out << std::quoted(k) << ':' << (*src)[k] << ',';
		out << std::quoted(last) << ':' << (*src)[last] ;
	}
	return out << '}';
}

std::ostream& absd::operator << (std::ostream& out, const data& src)
{
	return to_json(out)(src);
}

std::ostream& absd::operator << (std::ostream& out, const data_type& dt)
{
	return out << static_cast<int>(dt);
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
	return t!=data_type::array && t!=data_type::object && t!=data_type::empty;
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
