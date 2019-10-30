/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/same_type.hpp>

#include "ast.hpp"

namespace cppjinja {

struct parser_data {
	struct {
		std::string b= "<=";
		std::string e= "=>";
	} output;
	struct {
		std::string b= "<%";
		std::string e= "%>";
	} term;
	struct {
		std::string b="<#";
		std::string e="#>";
	} cmt;
};

template<typename T>
using iterator_concept =
	boost::mpl::vector
		< boost::type_erasure::bidirectional_iterator<>
		, boost::type_erasure::same_type
		  < boost::type_erasure::bidirectional_iterator<>::value_type
		  , T
		  >
		, boost::type_erasure::relaxed
		>;
using iter_wtype = boost::type_erasure::any<iterator_concept<const wchar_t>>;
using iter_stype = boost::type_erasure::any<iterator_concept<const char>>;

s_block parsers(iter_stype begin, iter_stype end, parser_data env={});
s_block parserw(iter_wtype begin, iter_wtype end, parser_data env={});
s_block parse(std::string_view data, parser_data env={});
s_block parse(std::wstring_view data, parser_data env={});
w_block wparse(std::wstring_view data, parser_data env={});

} // namespace cppjinja
