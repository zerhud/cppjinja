/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "abstract_data.hpp"

#include <cassert>

using abs_data = abstract_data::data;

abs_data::data(std::shared_ptr<abstract_data::data_holder> src)
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
