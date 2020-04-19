/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator tmpl_compiler

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/type_index.hpp>

#include "mocks.hpp"
#include "parser/parse.hpp"
#include "parser/grammar/tmpls.hpp"
#include "evtree/tmpl_compiler.hpp"

#include "evtree/node.hpp"
#include "evtree/nodes/tmpl.hpp"

namespace ast = cppjinja::ast;
namespace txt = cppjinja::text;
namespace nodes = cppjinja::evtnodes;
using namespace std::literals;
using mocks::mock_exenv_fixture;

using cppjinja::evt::compiled_tmpl;

template<typename Cnt, typename T>
bool check_find(const Cnt& cnt, const T& obj)
{
	auto pos = std::find(std::begin(cnt), std::end(cnt), obj);
	return pos != std::end(cnt);
}

template<typename Cnt>
bool check_link(const Cnt& cnt, cppjinja::evt::node* p, cppjinja::evt::node* c)
{
	return check_find(cnt, cppjinja::evt::edge{p, c});
}

std::string random_content()
{
	static std::string chars =
	        "abcdefghijklmnopqrstuvwxyz"
	        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	        ;
	std::random_device dev;
	std::uniform_int_distribution<> dest(0, chars.size());
	std::string ret;
	std::size_t sz = dest(dev) + 1;
	for(std::size_t i=0;i<sz;++i) ret += chars[dest(dev)];
	return ret;
}

template<typename Cnt>
std::string make_node_types_str(const Cnt& cnt)
{
	std::string ret;
	BOOST_REQUIRE(!cnt.empty());
	for(auto& n:cnt)
	{
		BOOST_REQUIRE(n);
		std::string t = boost::typeindex::type_id_runtime(*n.get()).pretty_name();
		assert(t.substr(0,20) == "cppjinja::evtnodes::");
		ret += t.erase(0,20) + ","s;
	}
	ret.back() = '.';
	return ret;
}

cppjinja::evt::compiled_tmpl build_tree(std::string_view ptxt)
{
	ast::file parsed;
	BOOST_TEST_CONTEXT("parsing: " << ptxt)
	    BOOST_REQUIRE_NO_THROW( parsed = txt::parse(txt::file, ptxt) );
	assert(parsed.tmpls.size() == 1);
	return cppjinja::evt::tmpl_compiler()(parsed.tmpls[0]);
}

BOOST_AUTO_TEST_SUITE(tmpl_compiler)

BOOST_AUTO_TEST_CASE(empty_tmpl)
{
	cppjinja::ast::tmpl tmpl;
	compiled_tmpl tree = cppjinja::evt::tmpl_compiler()(tmpl);
	BOOST_TEST(tree.tmpl_name == "");
	BOOST_TEST(make_node_types_str(tree.nodes) == "tmpl,block_named.");
	BOOST_TEST(tree.lrnd.size() == 0);
	BOOST_TEST(tree.lctx.size() == 1);
	BOOST_CHECK(check_link(tree.lctx, tree.nodes[0].get(), tree.nodes[1].get()));
}

BOOST_FIXTURE_TEST_CASE(single_string, mock_exenv_fixture)
{
	auto cnt = random_content();
	compiled_tmpl tree = build_tree(cnt);

	BOOST_TEST(tree.tmpl_name == "");
	BOOST_TEST(make_node_types_str(tree.nodes) == "tmpl,block_named,content.");

	MOCK_EXPECT(env.current_node).once();
	MOCK_EXPECT(env.crinfo).returns(cppjinja::evt::render_info{false,false});
	BOOST_CHECK_NO_THROW(tree.nodes[2]->render(env));
	BOOST_TEST(out.str() == cnt);

	BOOST_TEST(tree.lrnd.size() == 1);
	BOOST_CHECK(check_link(tree.lrnd, tree.nodes[1].get(), tree.nodes[2].get()));

	BOOST_CHECK(check_link(tree.lctx, tree.nodes[0].get(), tree.nodes[1].get()));
	BOOST_CHECK(check_link(tree.lctx, tree.nodes[1].get(), tree.nodes[2].get()));
}

BOOST_FIXTURE_TEST_CASE(inner_block, mock_exenv_fixture)
{
	auto cnt = random_content();
	auto data = "<% block a %>" + cnt + "<% endblock %>";
	compiled_tmpl tree = build_tree(data);

	BOOST_TEST(tree.tmpl_name == "");
	BOOST_TEST(make_node_types_str(tree.nodes) == "tmpl,block_named,op_out,block_named,content.");
}

BOOST_AUTO_TEST_SUITE_END() // tmpl_compiler
