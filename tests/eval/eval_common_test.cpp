/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "evaluator/eval.hpp"

#include "parser/grammar/tmpls.hpp"

#include "mocks.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;
using namespace std::literals;

std::string parse_single(std::string_view data, cppjinja::data_provider& prov)
{
	std::stringstream result;

	BOOST_TEST_CONTEXT("data is " << data)
	{
		ast::tmpl tmpl = txt::parse(txt::tmpl, data);
		cppjinja::evaluator ev({tmpl});

		BOOST_CHECK_NO_THROW( ev.render(result, prov) );
	}

	return result.str();
}

BOOST_AUTO_TEST_CASE(string)
{
	auto data = std::make_unique<mocks::data_provider>();
	BOOST_TEST( parse_single("content"sv, *data) == "content"s );
}

BOOST_AUTO_TEST_CASE(number)
{
	auto data = std::make_unique<mocks::data_provider>();
	BOOST_TEST( parse_single("10"sv, *data) == "10"s );
}

BOOST_AUTO_TEST_CASE(no_tmpls)
{
	BOOST_CHECK_THROW( cppjinja::evaluator({}), std::runtime_error );
}
