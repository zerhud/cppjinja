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
#include <compare>
#include <optional>
#include <memory_resource>

namespace absd {

class data;
class to_json_printer;

template<typename T>
concept IntegerFloating = 
        std::is_same_v<std::decay_t<T>, float>
     || std::is_same_v<std::decay_t<T>, double>;
template<typename T>
concept Boolean = std::is_same_v<std::decay_t<T>, bool>;
template<typename T>
concept Integer = std::integral<std::decay_t<T>> && !Boolean<T> && !IntegerFloating<T>;
template<typename T>
concept TrivialData = IntegerFloating<T> || Integer<T> || Boolean<T>;
template<typename T>
concept StringData = std::is_constructible_v<std::pmr::string,T>;
template<typename T>
concept Array = std::is_same_v<std::decay_t<T>, std::pmr::vector<data>>;
template<typename T>
concept Object = std::is_same_v<std::decay_t<T>, std::pmr::map<std::pmr::string,data>>;
template<typename T>
concept AnyData = TrivialData<T> || StringData<T> || Array<T> || Object<T>;

using data_variant = std::variant<std::int64_t, double, std::pmr::string, bool>;
enum class data_type {
	integer, floating_point, string, boolean,
	object, array,
	empty };

struct reflection_info {
	data_type type;
	std::uint64_t size=0;
	std::pmr::vector<std::string_view> keys{};

	auto operator <=> (const reflection_info&) const noexcept =default ;
};

std::ostream& operator << (std::ostream& out, const data& src);
std::ostream& operator << (std::ostream& out, const data_type& dt);

std::partial_ordering operator <=> (const data& l, const data& r) noexcept ;
template<Integer T>
std::strong_ordering operator <=> (const data& l, T&& r)
{
	return ((std::int64_t)l) <=> r;
}
template<TrivialData T>
auto operator <=> (const data& l, T&& r)
{
	return ((std::decay_t<T>)l) <=> r;
}
std::strong_ordering operator <=> (const data& l, std::string_view r);

template<typename T>
	requires AnyData<T> || std::is_same_v<std::decay_t<T>,data>
bool operator == (const data& l, T&& r) { return l<=>r == 0; }

class data_holder {
public:
	typedef std::shared_ptr<data_holder> self_type;
	virtual ~data_holder() noexcept =default;

	virtual std::pmr::memory_resource* storage() const =0 ;
	virtual reflection_info reflect() const =0;

	virtual std::int64_t to_int() const =0;
	virtual double to_double() const =0;
	virtual std::pmr::string to_string() const =0;
	virtual bool to_bool() const =0;

	virtual self_type by_key(std::string_view key) const =0;
	virtual self_type by_ind(std::int64_t ind) const =0;

	virtual self_type at_key(std::string_view key) const =0;
	virtual self_type at_ind(std::int64_t ind) const =0;
};

class data final {
	friend class to_json_printer;
	friend std::ostream& operator << (std::ostream&, const data&);
	friend std::partial_ordering operator <=> (const data&, const data&) noexcept ;
	using end_cache_t = data_variant;

	std::shared_ptr<data_holder> source;

	mutable std::optional<reflection_info> reflect;
	mutable std::optional<end_cache_t> cache;
	mutable std::pmr::map<std::pmr::string,data> key_cache;
	mutable std::pmr::map<std::int64_t,data,std::less<>> ind_cache;

	bool is_cached() const ;
	static bool is_pod(const reflection_info& info) ;
	const reflection_info& ref_info() const ;
public:
	data(std::shared_ptr<data_holder> src);

	std::shared_ptr<data_holder> src() const ;

	bool is_string() const ;
	bool is_integer() const ;
	bool is_float() const ;
	bool is_boolean() const ;
	bool is_object() const ;
	bool is_array() const ;
	bool is_empty() const ;

	operator std::int64_t () const ;
	operator double () const ;
	operator std::pmr::string () const ;
	explicit operator bool () const ;
	operator std::pmr::vector<data>() const
	{
		return as_array();
	}
	operator std::pmr::map<std::pmr::string,data>() const
	{
		return as_map();
	}

	inline std::pmr::string str() const
	{
		return (std::pmr::string)(*this);
	}

	inline std::string hstr() const
	{
		auto s = str();
		return std::string(s.begin(),s.end());
	}

	std::pmr::map<std::pmr::string,data> as_map() const ;
	std::pmr::vector<data> as_array() const ;

	const data& operator [] (std::string_view key) const ;
	const data& operator [] (std::int64_t ind) const ;
	template<StringData T>
	const data& operator [] (const T& key) const
	{
		return (*this)[std::string_view(key)];
	}
	template<Integer T>
	const data& operator [] (T key) const
	{
		return (*this)[(std::int64_t)key];
	}
};

class to_json_printer final {
	std::ostream& out;
	reflection_info info;
	const data* src;
	bool is_json = false;

	std::ostream& prt();
	std::ostream& prt(const data& obj);
	std::ostream& str(const std::pmr::string& s) ;
	std::ostream& cached() ;
	std::ostream& pod() ;
	std::ostream& array() ;
	std::ostream& object() ;
	std::ostream& pbool() ;
public:
	to_json_printer(std::ostream& out);
	std::ostream& operator()(const data& to_print) ;
};

} // namespace absd
