/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_tmpls

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/grammar/tmpls.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;

BOOST_AUTO_TEST_CASE(extend_st)
{
	std::string data = "<% extends 'kuku.tmpl' %>";
	ast::extend_st result;

	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::extend_st, data) );
	BOOST_TEST( result.file_name == "kuku.tmpl" );
	BOOST_TEST( !result.tmpl_name.has_value() );

	data = "<% extends template 'tmpl' from 'kuku.tmpl' %>";
	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::extend_st, data) );
	BOOST_TEST( result.file_name == "kuku.tmpl" );
	BOOST_TEST( result.tmpl_name.has_value() );
	BOOST_TEST( *result.tmpl_name == "tmpl" );
}
