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
}

BOOST_AUTO_TEST_CASE(original_file)
{
	std::string data = "<% extends 'kuku.tmpl' %>content";
	ast::file result;

	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::file, data) );
	BOOST_TEST( result.name.empty() );
	BOOST_TEST_REQUIRE( result.tmpls.size() == 1);

	ast::tmpl& tmpl = result.tmpls[0];
	BOOST_TEST( tmpl.name.empty() );
	BOOST_TEST_REQUIRE( tmpl.extends.size() == 1 );
	BOOST_TEST( tmpl.extends[0].file_name == "kuku.tmpl" );
	BOOST_TEST( !tmpl.extends[0].tmpl_name.has_value() );
	BOOST_TEST_REQUIRE( tmpl.content.size() == 1 );

	data = "<% extends 'kuku.tmpl' %>content<% extends 'kuku.tmpl' %>";
	BOOST_REQUIRE_THROW( result = txt::parse(txt::file, data), std::exception );
}

BOOST_AUTO_TEST_CASE(ex_file)
{
	std::string data = "<% template tmpl extends kuku from 'kuku.tmpl' %>content<% endtemplate %>";
	ast::file result;

	BOOST_REQUIRE_NO_THROW( result = txt::parse(txt::file, data) );
	BOOST_TEST( result.name.empty() );
	BOOST_TEST_REQUIRE( result.tmpls.size() == 1);

	ast::tmpl& tmpl = result.tmpls[0];
	BOOST_TEST_REQUIRE( tmpl.extends.size() == 1 );
	BOOST_TEST( tmpl.extends[0].file_name == "kuku.tmpl" );
	BOOST_TEST_REQUIRE( tmpl.extends[0].tmpl_name.has_value() );
	BOOST_TEST( *tmpl.extends[0].tmpl_name == "kuku" );
	BOOST_TEST_REQUIRE( tmpl.content.size() == 1 );
}