/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <optional>
#include <memory_resource>

namespace absd {

class data;
class to_json_printer;

using data_variant = std::variant<std::int64_t, double, std::pmr::string, bool>;
enum class data_type { integer, floating_point, string, boolean, object, array, empty };
struct reflection_info {
	data_type type;
	std::uint64_t size;
	std::pmr::vector<std::string_view> keys;
};

std::ostream& operator << (std::ostream& out, const data& src);
std::ostream& operator << (std::ostream& out, const data_type& dt);

class data_holder {
public:
	typedef std::shared_ptr<data_holder> self_type;
	virtual ~data_holder() noexcept =default;

	virtual reflection_info reflect() const =0;

	virtual std::int64_t to_int() const =0;
	virtual double to_double() const =0;
	virtual std::pmr::string to_string() const =0;
	virtual bool to_bool() const =0;

	virtual self_type by_key(std::string_view key) const =0;
	virtual self_type by_ind(std::int64_t ind) const =0;
};

class data final {
	friend class to_json_printer;
	friend std::ostream& operator << (std::ostream& out, const data& src);
	using end_cache_t = data_variant;

	std::shared_ptr<data_holder> source;

	mutable std::optional<end_cache_t> cache;
	mutable std::map<std::string,data,std::less<>> key_cache;
	mutable std::map<std::int64_t,data,std::less<>> ind_cache;

	bool is_cached() const ;
	static bool is_pod(const reflection_info& info) ;
public:
	data(std::shared_ptr<data_holder> src);

	explicit operator std::int64_t () const ;
	explicit operator double () const ;
	explicit operator std::pmr::string () const ;
	explicit operator bool () const ;

	inline std::pmr::string str() const
	{
		return (std::pmr::string)(*this);
	}

	const data& operator [] (std::string_view key) const ;
	const data& operator [] (std::int64_t ind) const ;
};

class to_json_printer final {
	std::ostream& out;
	reflection_info info;
	const data* src;

	std::ostream& cached() ;
	std::ostream& pod() ;
	std::ostream& array() ;
	std::ostream& object() ;
	std::ostream& pbool() ;
public:
	to_json_printer(std::ostream& out);
	std::ostream& operator()(const data& to_print) ;
};

class simple_data_holder : public data_holder {
	typedef std::pmr::polymorphic_allocator<simple_data_holder> alloc_t;

	mutable std::shared_ptr<std::pmr::memory_resource> mem;
	std::optional<data_variant> pod;
	std::pmr::map<std::pmr::string,std::shared_ptr<simple_data_holder>,std::less<>> object;
	std::pmr::vector<std::shared_ptr<simple_data_holder>> array;

	bool is_setted() const ;
	void require_change() const ;
	void require_extract_pod() const ;
	void require_extract_obj() const ;
	void require_extract_arr() const ;
	void set_int(std::int64_t v);
public:
	simple_data_holder();
	simple_data_holder(std::shared_ptr<std::pmr::memory_resource> mem);
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
