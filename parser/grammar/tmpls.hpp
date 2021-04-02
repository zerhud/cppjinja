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

	struct extend_st_class;
	struct extend_st_ex_class;

	struct tmpl_class;
	struct tmpl_ex_class;
	struct tmpl_original_class;

	struct file_class;

	const x3::rule<extend_st_class, ast::extend_st> extend_st = "extend_st";
	const x3::rule<extend_st_ex_class, ast::extend_st> extend_st_ex = "extend_st_ex";

	const x3::rule<tmpl_class, ast::tmpl> tmpl = "tmpl";
	const x3::rule<tmpl_ex_class, ast::tmpl> tmpl_ex = "tmpl_ex";
	const x3::rule<tmpl_original_class, ast::tmpl> tmpl_original = "tmpl_original";

	const x3::rule<file_class, ast::file> file = "file";

	BOOST_SPIRIT_DECLARE(decltype(extend_st))
	BOOST_SPIRIT_DECLARE(decltype(extend_st_ex))

	BOOST_SPIRIT_DECLARE(decltype(tmpl))
	BOOST_SPIRIT_DECLARE(decltype(tmpl_ex))
	BOOST_SPIRIT_DECLARE(decltype(tmpl_original))

	BOOST_SPIRIT_DECLARE(decltype(file))

} // namespace cppjinja::text

