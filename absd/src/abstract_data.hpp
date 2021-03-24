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

class data_holder {
public:
	typedef std::unique_ptr<data_holder> self_type;
	virtual ~data_holder() noexcept =default;

	virtual std::int64_t to_int() const =0;
	virtual double to_double() const =0;
	virtual std::pmr::string to_string() const =0;
	virtual bool to_bool() const =0;

	virtual self_type by_key(std::string_view key) const =0;
	virtual self_type by_ind(std::int64_t ind) const =0;
};

class data final {
	using end_cache_t = std::variant<std::int64_t, double, std::pmr::string, bool>;

	std::shared_ptr<data_holder> source;

	mutable std::optional<end_cache_t> cache;
	mutable std::map<std::string,data,std::less<>> key_cache;
	mutable std::map<std::int64_t,data,std::less<>> ind_cache;

	bool is_cached() const ;
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

} // namespace absd
