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
bool operator == (const data& l, const data& r) noexcept ;

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

	bool is_string() const ;
	bool is_integer() const ;
	bool is_float() const ;
	bool is_boolean() const ;
	bool is_object() const ;
	bool is_array() const ;

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

} // namespace absd
