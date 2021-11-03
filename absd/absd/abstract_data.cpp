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
	return prt(to_print);
}
std::ostream& to_json::prt(const data& obj)
{
	struct swapper {
		const data* old;
		to_json* self;
		swapper(to_json* self, const data& obj)
		    : old(self->src)
		    , self(self)
		{
			self->src = &obj;
			self->info  = obj.source->reflect();
		}
		~swapper() noexcept
		{
			self->src = old;
			self->info  = old->source->reflect();
		}
	} swp{this, obj};
	return prt();
}
std::ostream& to_json::prt()
{
	if(src->is_pod(info)) return src->is_cached() ? cached() : pod();
	if(info.type == data_type::array) return array();
	if(info.type == data_type::object) return object();
	if(info.type == data_type::empty) return out;
	assert(false);
	throw std::logic_error("not all data types cached");
}
std::ostream& to_json::cached()
{
	std::visit( overloaded {
	     [this](auto&& d){out << d;},
	     [this](bool){pbool();},
	     [this](std::pmr::string& d){str(d);}
	    }, src->cache.value());
	return out;
}
std::ostream& to_json::pod()
{
	if(info.type == data_type::integer) return out << (std::int64_t)*src;
	if(info.type == data_type::floating_point) return out << (double)*src;
	if(info.type == data_type::string) return str(src->str());
	if(info.type == data_type::boolean) return pbool();
	assert(false);
	throw std::logic_error("not all data types cached");
}
std::ostream& to_json::str(const std::pmr::string& s)
{
	if(is_json) return out << std::quoted(s);
	return out << s;
}
std::ostream& to_json::pbool()
{
	return out << (*src?"true":"false");
}
std::ostream& to_json::array()
{
	is_json = true;
	out << '[';
	if(info.size != 0) prt((*src)[0]);
	for(std::uint64_t i=1;i<info.size;++i)
		out << ',', prt((*src)[i]);
	return out << ']';
}
std::ostream& to_json::object()
{
	is_json = true;
	out << '{';
	if(!info.keys.empty()) {
		auto last = info.keys.back();
		info.keys.pop_back();
		for(auto& k:info.keys)
			out << std::quoted(k) << ':', prt((*src)[k]) << ',';
		out << std::quoted(last) << ':', prt((*src)[last]) ;
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

std::partial_ordering absd::operator <=> (const data& l, const data& r) noexcept
{
	using ord = std::partial_ordering;
	if(l.source == r.source) return ord::equivalent;
	const reflection_info& ri = l.ref_info();
	if(ri != r.ref_info()) return ri <=> r.ref_info();
	if(l.is_pod(ri)) {
		if(l.is_string()) return l.str() <=> r.str();
		if(l.is_integer()) return (std::int64_t)l <=> (std::int64_t)r;
		if(l.is_float()) return (double)l <=> (double)r;
		if(l.is_boolean()) return (bool)l <=> (bool)r;
		assert(false);
		return ord::unordered;
	}
	if(l.is_array()) {
		if(ri.size != r.ref_info().size)
			return ri.size <=> r.ref_info().size;
		for(std::size_t i=0;i<ri.size;++i) {
			auto res = l[i] <=> r[i];
			if(std::is_neq(res)) return res;
		}
		return ord::equivalent;
	}
	if(l.is_object()) {
		if(ri.keys != r.ref_info().keys)
			return ri.keys <=> r.ref_info().keys;
		for(auto& key:ri.keys) {
			auto res = l[key] <=> r[key];
			if(std::is_neq(res)) return res;
		}
		return ord::equivalent;
	}
	return ord::unordered;
}

std::strong_ordering absd::operator <=> (const data& l, std::string_view r)
{
	return l.str() <=> r;
}

absd::data::data(std::shared_ptr<absd::data_holder> src)
    : source(std::move(src))
    , key_cache(source->storage())
    , ind_cache(source->storage())
{
}

std::shared_ptr<absd::data_holder> absd::data::src() const
{
	return source;
}

bool absd::data::is_string() const
{
	assert(source);
	return ref_info().type == data_type::string;
}

bool absd::data::is_integer() const
{
	assert(source);
	return ref_info().type == data_type::integer;
}

bool absd::data::is_float() const
{
	assert(source);
	return ref_info().type == data_type::floating_point;
}

bool absd::data::is_boolean() const
{
	assert(source);
	return ref_info().type == data_type::boolean;
}

bool absd::data::is_object() const
{
	assert(source);
	return ref_info().type == data_type::object;
}

bool absd::data::is_array() const
{
	assert(source);
	return ref_info().type == data_type::array;
}

bool absd::data::is_cached() const
{
	return
	        cache.has_value()
	     || !key_cache.empty()
	     || !ind_cache.empty()
	        ;
}

bool absd::data::is_pod(const reflection_info& info)
{
	data_type t = info.type;
	return t!=data_type::array && t!=data_type::object && t!=data_type::empty;
}

const absd::reflection_info& absd::data::ref_info() const
{
	assert(source);
	if(!reflect) reflect = source->reflect();
	return *reflect;
}

absd::data::operator std::int64_t() const
{
	assert(source);
	if(!is_cached()) cache = source->to_int();
	return std::get<std::int64_t>(cache.value());
}

absd::data::operator double() const
{
	assert(source);
	if(!is_cached()) cache = source->to_double();
	return std::get<double>(cache.value());
}

absd::data::operator std::pmr::string() const
{
	assert(source);
	if(!is_cached()) cache = source->to_string();
	return std::get<std::pmr::string>(cache.value());
}

absd::data::operator bool() const
{
	assert(source);
	if(!is_cached()) cache = source->to_bool();
	return std::get<bool>(cache.value());
}

const absd::data& absd::data::operator [](std::string_view key) const
{
	assert(source);
	std::pmr::string skey(key, source->storage());
	if(is_cached() && key_cache.contains(skey))
		return key_cache.at(skey);
	key_cache.emplace(std::make_pair(skey, absd::data(source->by_key(key))));
	return key_cache.at(skey);
}

const absd::data& absd::data::operator [](std::int64_t ind) const
{
	if(is_cached() && ind_cache.contains(ind))
		return ind_cache.at(ind);
	ind_cache.emplace(std::make_pair(ind, absd::data(source->by_ind(ind))));
	return ind_cache.at(ind);
}

std::pmr::map<std::pmr::string,absd::data> absd::data::as_map() const
{
	auto keys = ref_info().keys;
	for(auto& key:keys) (*this)[key];
	return key_cache;
}

std::pmr::vector<absd::data> absd::data::as_array() const
{
	assert(source);
	std::int64_t size = ref_info().size;
	std::pmr::vector<absd::data> ret(source->storage());
	for(std::int64_t i=0;i<size;++i) ret.emplace_back((*this)[i]);
	return ret;
}
