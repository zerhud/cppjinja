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
namespace tdata = boost::unit_test::data;
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

template<typename Node>
std::string make_node_type_str(Node* n)
{
	std::string t = boost::typeindex::type_id_runtime(*n).pretty_name();
	assert(t.substr(0,20) == "cppjinja::evtnodes::");
	return t.erase(0,20);
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

template<typename T>
std::string make_node_seq_str(
          cppjinja::evt::node* p
        , const std::vector<cppjinja::evt::edge<T>>& edges)
{
	std::string ret = make_node_type_str(p);
	for(auto& edge:edges) if(edge.parent == p) {
		ret += ',' + make_node_seq_str(edge.child, edges);
	}
	return ret + '.';
}

void check_main(compiled_tmpl& t, std::string name, std::size_t blocks, int cnt)
{
	BOOST_TEST(t.tmpl_name == name);
	BOOST_TEST(dynamic_cast<nodes::tmpl*>(t.tmpl_node()) != nullptr);
	BOOST_TEST(dynamic_cast<nodes::block_named*>(t.main_block()) != nullptr);
	BOOST_TEST(make_node_seq_str(t.tmpl_node(), t.lrnd).substr(0,16) == "tmpl,block_named");
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

BOOST_AUTO_TEST_SUITE(phase_compilation)

BOOST_AUTO_TEST_SUITE(tmpl_compiler)

BOOST_AUTO_TEST_CASE(empty_tmpl)
{
	cppjinja::ast::tmpl tmpl;
	compiled_tmpl tree = cppjinja::evt::tmpl_compiler()(tmpl);
	check_main(tree, "", 1, 0);
	BOOST_TEST(tree.lrnd.size() == 1);
}

BOOST_DATA_TEST_CASE_F(
          mock_exenv_fixture, content
        , tdata::make(""s, "<%raw%>"s)
        ^ tdata::make(""s, "<%endraw%>"s)
        ^ tdata::make(random_content(), "<= 'a' =>"s)
        , begin, end, cnt)
{
	compiled_tmpl tree = build_tree(begin + cnt + end);
	check_main(tree, "", 1, 1);
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named,content..");

	nodes::content* cntn = nullptr;
	for(auto& n:tree.nodes)
		if((cntn = dynamic_cast<nodes::content*>(n.get()))) break;
	BOOST_REQUIRE(cntn != nullptr);
	MOCK_EXPECT(env.current_node).once();
	MOCK_EXPECT(env.crinfo).returns(cppjinja::evt::render_info{false,false});
	BOOST_CHECK_NO_THROW(cntn->render(env));
	BOOST_TEST(out.str() == cnt);
}

BOOST_FIXTURE_TEST_CASE(inner_block, mock_exenv_fixture)
{
	auto cnt = random_content();
	auto data = "<% block a %>" + cnt + "<% endblock %><%block b%>b<%endblock%>";
	compiled_tmpl tree = build_tree(data);
	check_main_nodes(tree.nodes, 3, 2);
	nodes::callable* anode = nullptr;
	nodes::callable* bnode = nullptr;
	for(auto& r:tree.roots) if(r->name() == "a") anode = r;
	for(auto& r:tree.roots) if(r->name() == "b") bnode = r;
	BOOST_REQUIRE(anode);
	auto mbrnd = make_node_seq_str(tree.main_block(), tree.lrnd);
	BOOST_TEST(mbrnd ==
	           "block_named,block_named,content..,block_named,content...");
	BOOST_TEST(make_node_seq_str(anode, tree.lrnd) == "block_named,content..");
	BOOST_TEST(make_node_seq_str(bnode, tree.lrnd) == "block_named,content..");
}

BOOST_AUTO_TEST_CASE(named_with_params)
{
	compiled_tmpl tree = build_tree("<%block a(p)%>cnt<%endblock%>");
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named.");
	tree = build_tree("<%block a(p=1)%>cnt<%endblock%>");
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named,block_named,content...");
}

BOOST_FIXTURE_TEST_CASE(op_out, mock_exenv_fixture)
{
	auto data = "<= a =>";
	compiled_tmpl tree = build_tree(data);
	check_main(tree, "", 1, 0);
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named,op_out..");

	expect_glp(0, 1, 1);
	locals.add("a", std::make_shared<cppjinja::evt::var_solver>(ast::value_term{""}));
	MOCK_EXPECT(env.current_node);
	make_node_seq(tree.main_block(), tree.lrnd)[1]->render(env);
}

BOOST_FIXTURE_TEST_CASE(op_set, mock_exenv_fixture)
{
	auto data = "<% set a = 'a' %>";
	compiled_tmpl tree = build_tree(data);
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named,op_set..");
}

BOOST_FIXTURE_TEST_CASE(block_macro, mock_exenv_fixture)
{
	compiled_tmpl tree = build_tree("cnt<% macro a %>in<%set a='a'%><% endmacro %>c");
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named,content.,content..");
	BOOST_TEST(tree.roots.size() == 2);

	nodes::callable* macro = nullptr;
	for(auto& r:tree.roots) if(r->name()=="a") macro = r;
	BOOST_REQUIRE(macro);
	BOOST_TEST(make_node_seq_str(macro, tree.lrnd) == "block_macro,content.,op_set..");
}

BOOST_FIXTURE_TEST_CASE(if_only, mock_exenv_fixture)
{
	compiled_tmpl tree = build_tree("<% if 1==1 +%>cnt<% endif +%>c");
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) == "block_named,block_if,content_block,content...,content..");
	auto mb_rnd = make_node_seq(tree.main_block(), tree.lrnd);
	BOOST_TEST(mb_rnd[2]->rinfo().trim_left == true);
	BOOST_TEST(mb_rnd[2]->rinfo().trim_right == false);
}

BOOST_FIXTURE_TEST_CASE(if_else, mock_exenv_fixture)
{
	compiled_tmpl tree = build_tree("<% if 1==1 +%><%+ else %><% set a='a' %><% endif +%>cnt");
	BOOST_TEST(make_node_seq_str(tree.main_block(), tree.lrnd) ==
	           "block_named,block_if,content_block.,content_block,op_set...,content..");
	auto mb_rnd = make_node_seq(tree.main_block(), tree.lrnd);
	BOOST_TEST(mb_rnd[2]->rinfo().trim_left == true);
	BOOST_TEST(mb_rnd[2]->rinfo().trim_right == true);
	BOOST_TEST(mb_rnd[3]->rinfo().trim_left == false);
	BOOST_TEST(mb_rnd[3]->rinfo().trim_right == false);
}

BOOST_AUTO_TEST_CASE(tmpl_name)
{
	compiled_tmpl tree = build_tree("<% template kuku %>c<%endtemplate%>");
	BOOST_TEST(tree.tmpl_name == "kuku");
	tree = build_tree("<% template kuku extends base%>c<%endtemplate%>");
	BOOST_REQUIRE(tree.extends.size() == 1);
	BOOST_REQUIRE(tree.extends[0].tmpl_name);
	BOOST_TEST(*tree.extends[0].tmpl_name == "base");
}

BOOST_AUTO_TEST_SUITE_END() // tmpl_compiler

BOOST_AUTO_TEST_SUITE_END() // phase_compilation
