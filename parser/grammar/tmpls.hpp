/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/tmpls.hpp"

namespace cppjinja::text {

	namespace x3 = boost::spirit::x3;

	class extend_st_class;

	const x3::rule<extend_st_class, ast::extend_st> extend_st = "extend_st";

	BOOST_SPIRIT_DECLARE(decltype(extend_st))

} // namespace cppjinja::text

