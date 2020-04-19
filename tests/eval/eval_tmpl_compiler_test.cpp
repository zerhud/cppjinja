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
#include "evtree/nodes/content.hpp"
#include "evtree/nodes/block_named.hpp"

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

template<typename Cnt, typename T>
bool check_find_type(const Cnt& cnt)
{
	auto pos = std::find_if(std::begin(cnt), std::end(cnt), [](auto& n){
		return dynamic_cast<const T*>(n.get()) != nullptr;
	});
	return pos != std::end(cnt);
}

template<typename C>
void check_main_nodes(const C& cont, int blocks, int cnt)
{
	auto check_tmpl = [](const auto& n){return dynamic_cast<const nodes::tmpl*>(n.get());};
	auto check_cnt = [](const auto& n){return dynamic_cast<const nodes::content*>(n.get());};
	auto check_block = [](const auto& n){return dynamic_cast<const nodes::block_named*>(n.get());};
	BOOST_TEST(std::count_if(std::begin(cont), std::end(cont), check_cnt) == cnt);
	BOOST_TEST(std::count_if(std::begin(cont), std::end(cont), check_tmpl) == 1);
	BOOST_TEST(std::count_if(std::begin(cont), std::end(cont), check_block) == blocks);
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
		std::string t = boost::typeindex::type_id_runtime(*n).pretty_name();
		assert(t.substr(0,20) == "cppjinja::evtnodes::");
		ret += t.erase(0,20) + ","s;
	}
	ret.back() = '.';
	return ret;
}

template<typename T>
std::vector<cppjinja::evt::node*>
make_node_seq(cppjinja::evt::node* p, const std::vector<cppjinja::evt::edge<T>>& edges)
{
	std::vector<cppjinja::evt::node*> ret;
	ret.emplace_back(p);
	for(auto& edge:edges) if(edge.parent == p) {
		auto childrend = make_node_seq(edge.child, edges);
		for(auto& child:childrend) ret.emplace_back(child);
	}
	return ret;
}

void check_main(compiled_tmpl& t, std::string name, std::size_t blocks, int cnt)
{
	BOOST_TEST(t.tmpl_name == name);
	BOOST_TEST(dynamic_cast<nodes::tmpl*>(t.tmpl_node()) != nullptr);
	BOOST_TEST(dynamic_cast<nodes::block_named*>(t.main_block()) != nullptr);
	BOOST_TEST(make_node_types_str(make_node_seq(t.tmpl_node(), t.lrnd)) == "tmpl.");
	BOOST_CHECK(check_link(t.lctx, t.tmpl_node(), t.main_block()));
	BOOST_TEST(make_node_types_str(make_node_seq(t.tmpl_node(), t.lctx)) == "tmpl,block_named.");
	BOOST_TEST(t.roots.size() == blocks );
	check_main_nodes(t.nodes, blocks, cnt);
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
	check_main(tree, "", 1, 0);
	BOOST_TEST(tree.lrnd.size() == 0);
}

BOOST_FIXTURE_TEST_CASE(single_string, mock_exenv_fixture)
{
	auto cnt = random_content();
	compiled_tmpl tree = build_tree(cnt);
	check_main(tree, "", 1, 1);
	BOOST_TEST(make_node_types_str(make_node_seq(tree.main_block(), tree.lrnd)) == "block_named,content.");

	MOCK_EXPECT(env.current_node).once();
	MOCK_EXPECT(env.crinfo).returns(cppjinja::evt::render_info{false,false});
	BOOST_CHECK_NO_THROW(tree.nodes[2]->render(env));
	BOOST_TEST(out.str() == cnt);

}

BOOST_FIXTURE_TEST_CASE(inner_block, mock_exenv_fixture)
{
	auto cnt = random_content();
	auto data = "<% block a %>" + cnt + "<% endblock %>";
	compiled_tmpl tree = build_tree(data);
	check_main(tree, "", 2, 1);
	nodes::callable* anode = nullptr;
	for(auto& r:tree.roots) if(r->name() == "a") anode = r;
	BOOST_REQUIRE(anode);
	BOOST_TEST(make_node_types_str(make_node_seq(anode, tree.lrnd)) == "block_named,content.");
	BOOST_TEST(make_node_types_str(make_node_seq(tree.main_block(), tree.lrnd)) == "block_named,op_out.");
}

BOOST_AUTO_TEST_SUITE_END() // tmpl_compiler
