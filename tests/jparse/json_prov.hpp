/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/json.hpp>
#include <eval/eval.hpp>

namespace cogen {

using storage_ptr = std::shared_ptr<std::pmr::memory_resource>;

class json_data_holder : public absd::data_holder {
	boost::json::value val;
	storage_ptr mem;
public:
	static self_type create_for_val(storage_ptr mem, boost::json::value v) ;

	json_data_holder(boost::json::value val, storage_ptr s);
	std::pmr::memory_resource* storage() const override ;
	absd::reflection_info reflect() const override;

	std::int64_t to_int() const override;
	double to_double() const override;
	std::pmr::string to_string() const override;
	bool to_bool() const override;

	self_type by_key(std::string_view key) const override;
	self_type by_ind(std::int64_t ind) const override;
};

class jinja_json_prov : public cppjinja::data_provider {
	boost::json::value jdata;
	storage_ptr mem;

	static std::string ref_to_str(const cppjinja::east::var_name& ref) ;
public:
	jinja_json_prov(boost::json::value jd);
	absd::data value(const cppjinja::east::var_name& val) const override ;
	absd::data value(const cppjinja::east::function_call& val) const override ;
};

} // namespace cogen
