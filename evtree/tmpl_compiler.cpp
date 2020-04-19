/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tmpl_compiler.hpp"

#include "nodes/tmpl.hpp"
#include "nodes/content.hpp"
#include "nodes/block_named.hpp"
#include "nodes/op_out.hpp"

typedef std::unique_ptr<cppjinja::evt::node> node_ptr;
typedef std::vector<node_ptr> vec_type;

cppjinja::evt::compiled_tmpl
cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::tmpl t)
{
	cur_tmpl = std::move(t);
	make_main_nodes();
	compile_template_content();
	return std::move(result);
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::string_t& cnt)
{
	assert(!ctx_stack.empty());
	assert(!rnd_stack.empty());
	node* created = result.nodes.emplace_back(
	            std::make_unique<evtnodes::content>(cnt)).get();
	result.lrnd.emplace_back(rnd_stack.back(), created);
}

void cppjinja::evt::tmpl_compiler::operator()(ast::forward_ast<ast::block_named>& obj)
{
	add_block(obj.get());
}

void cppjinja::evt::tmpl_compiler::compile_template_content()
{
	for(auto& c:cur_tmpl.content)
		boost::apply_visitor(*this, c.var);
}

void cppjinja::evt::tmpl_compiler::make_main_nodes()
{
	result.nodes.emplace_back(std::make_unique<evtnodes::tmpl>(cur_tmpl));
	add_block(ast::block_named{});
	result.lctx.emplace_back(result.tmpl_node(), result.main_block());
}

cppjinja::evtnodes::callable*
cppjinja::evt::tmpl_compiler::add_block(ast::block_named obj)
{
	auto bl = std::make_unique<evtnodes::block_named>(obj);
	evtnodes::callable* ret = bl.get();
	result.roots.emplace_back(bl.get());
	result.nodes.emplace_back(std::move(bl));
	ctx_stack.emplace_back(result.nodes.back().get());
	rnd_stack.emplace_back(result.nodes.back().get());

	//ast::op_out out;
	//result.nodes.emplace_back(std::make_unique<evtnodes::op_out>(out));

	for(auto& c:obj.content) boost::apply_visitor(*this, c.var);
	return ret;
}

cppjinja::evtnodes::tmpl* cppjinja::evt::compiled_tmpl::tmpl_node()
{
	if(nodes.empty()) return nullptr;
	auto ret = dynamic_cast<evtnodes::tmpl*>(nodes[0].get());
	assert(ret);
	return ret;
}

cppjinja::evtnodes::callable* cppjinja::evt::compiled_tmpl::main_block()
{
	if(nodes.size() < 2) return nullptr;
	auto ret = dynamic_cast<evtnodes::callable*>(nodes[1].get());
	assert(ret);
	return ret;
}
