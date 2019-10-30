/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#define BOOST_SPIRIT_X3_UNICODE

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>

namespace cppjinja::text {

	template<typename Id, typename Attribute>
	Attribute parse(boost::spirit::x3::rule<Id, Attribute> rule, std::string_view data)
	{
		//auto end = boost::u8_to_u32_iterator(data.end());
		//auto begin = boost::u8_to_u32_iterator(data.begin());
		auto end = data.end();
		auto begin = data.begin();

		Attribute result;
		bool success = boost::spirit::x3::parse(begin, end, rule, result);

		if(!success) throw std::runtime_error("cannot parse");

		return result;
	}

} // namespace cppjinja::text
