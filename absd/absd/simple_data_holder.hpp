/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "abstract_data.hpp"

namespace absd {

class simple_data_holder : public data_holder {
	typedef std::pmr::polymorphic_allocator<simple_data_holder> alloc_t;

	mutable std::shared_ptr<std::pmr::memory_resource> mem;
	std::optional<data_variant> pod;
	std::pmr::map<std::pmr::string,data,std::less<>> object;
	std::pmr::vector<data> array;
	bool is_empty_obj = false;
	bool is_empty_arr = false;

	void clear_state() ;
	void require_change() ;
	void require_extract_pod() const ;
	void require_extract_obj() const ;
	void require_extract_arr() const ;
	void set_int(std::int64_t v);
public:
	simple_data_holder();
	simple_data_holder(std::shared_ptr<std::pmr::memory_resource> m);

	template<TrivialData T>
	simple_data_holder(T v) : simple_data_holder() { (*this)=v; }
	template<TrivialData T>
	simple_data_holder(std::shared_ptr<std::pmr::memory_resource> m, T v)
	    : simple_data_holder(std::move(m)) { (*this)=v; }

	simple_data_holder(std::pmr::string v)
	    : simple_data_holder() { str() = std::move(v); }
	simple_data_holder(std::shared_ptr<std::pmr::memory_resource> m, std::pmr::string v)
	    : simple_data_holder(std::move(m)) { str() = std::move(v); }

	~simple_data_holder() noexcept =default ;

	std::pmr::memory_resource* storage() const override ;

	template<Integer T>
	simple_data_holder& operator = (T v)
	{
		set_int(v);
		return *this;
	}

	template<StringData T>
	simple_data_holder& operator = (T&& v)
	{
		str() = std::forward<T>(v);
		return *this;
	}

	simple_data_holder& operator = (double v);
	simple_data_holder& operator = (bool v);

	std::pmr::string& str();
	const std::pmr::string& str() const ;

	simple_data_holder& put(std::string_view key);
	void put(std::string_view key, data v);
	void make_empty_object();
	void make_empty_array();

	simple_data_holder& push_back();
	void push_back(data v);

	reflection_info reflect() const override;

	std::int64_t to_int() const override;
	double to_double() const override;
	std::pmr::string to_string() const override;
	bool to_bool() const override;

	self_type by_key(std::string_view key) const override;
	self_type by_ind(std::int64_t ind) const override;
};

template<typename T>
data make_simple(T&& d)
{
	return data{ std::make_shared<simple_data_holder>(std::forward<T>(d)) };
}

inline data operator "" _sd(const char* src, std::size_t size)
{
	return absd::data{std::make_shared<simple_data_holder>(
		            std::pmr::string(src,size))};
}
inline data operator "" _sd(unsigned long long int val)
{
	return absd::data{std::make_shared<simple_data_holder>(val)};
}

} // namespace absd
