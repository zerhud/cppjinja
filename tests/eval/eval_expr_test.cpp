/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator expr

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/exenv/expr_eval.hpp"
#include "parser/grammar/expr.hpp"
#include "parser/parse.hpp"

using namespace std::literals;


BOOST_AUTO_TEST_SUITE(phase_evaluate)
using mocks::mock_exenv_fixture;
BOOST_AUTO_TEST_SUITE(expr)
namespace txt = cppjinja::text;
namespace ext = cppjinja::text::expr_ops;
namespace est = cppjinja::ast::expr_ops;
using eeval = cppjinja::evt::expr_eval;
using cval = cppjinja::east::value_term;
BOOST_AUTO_TEST_SUITE(term)
BOOST_FIXTURE_TEST_CASE(string,  mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "'ok'"))->solve();
	BOOST_TEST(res == cval{"ok"s});
	res = eeval(&env)(txt::parse(ext::expr, "1"))->solve();
	BOOST_TEST(res == cval{1});
}
BOOST_FIXTURE_TEST_CASE(math,  mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "3+5-1*0.5"))->solve();
	BOOST_TEST(res == cval{7.5});
	eeval str_e(&env);
	BOOST_CHECK_THROW(str_e(txt::parse(ext::expr, "0.5+'1'")), std::runtime_error);

	res = eeval(&env)(txt::parse(ext::expr, "'ok' + 'ok'"))->solve();
	BOOST_TEST(res == cval{"okok"});

	eeval str2_e(&env);
	BOOST_CHECK_THROW(str2_e(txt::parse(ext::expr, "'ok'-'ups'")), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(concat, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "3~5~10*2"))->solve();
	BOOST_TEST(res == cval{"3520"});
}
BOOST_FIXTURE_TEST_CASE(negate, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "!!true"))->solve();
	BOOST_TEST(res == cval{true});
	eeval str_e(&env);
	BOOST_CHECK_THROW(str_e(txt::parse(ext::expr, "!'1'")), std::runtime_error);
}
BOOST_AUTO_TEST_SUITE_END() // term
BOOST_AUTO_TEST_SUITE_END() // expr
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
