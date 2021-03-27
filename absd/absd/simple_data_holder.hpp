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
	std::pmr::map<std::pmr::string,std::shared_ptr<simple_data_holder>,std::less<>> object;
	std::pmr::vector<std::shared_ptr<simple_data_holder>> array;

	void require_change() const ;
	void require_extract_pod() const ;
	void require_extract_obj() const ;
	void require_extract_arr() const ;
	void set_int(std::int64_t v);
public:
	simple_data_holder();
	simple_data_holder(std::shared_ptr<std::pmr::memory_resource> m);
	~simple_data_holder() noexcept =default ;

	std::pmr::memory_resource& storage() const ;

	template<typename T>
	requires (std::integral<T> && !std::is_same_v<T,bool> && !std::is_same_v<T,double>)
	simple_data_holder& operator = (T v)
	{
		set_int(v);
		return *this;
	}

	simple_data_holder& operator = (double v);
	simple_data_holder& operator = (bool v);

	std::pmr::string& str();
	const std::pmr::string& str() const ;

	simple_data_holder& put(std::string_view key);
	simple_data_holder& push_back();

	reflection_info reflect() const override;

	std::int64_t to_int() const override;
	double to_double() const override;
	std::pmr::string to_string() const override;
	bool to_bool() const override;

	self_type by_key(std::string_view key) const override;
	self_type by_ind(std::int64_t ind) const override;
};

} // namespace absd
