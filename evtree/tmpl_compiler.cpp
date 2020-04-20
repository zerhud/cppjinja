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
#include "nodes/op_set.hpp"
#include "nodes/block_macro.hpp"

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
	rnd_stack.emplace_back(result.tmpl_node());
	add_block(ast::block_named{});
	result.lctx.emplace_back(result.tmpl_node(), result.main_block());
}

cppjinja::evtnodes::callable*
cppjinja::evt::tmpl_compiler::add_block(ast::block_named obj)
{
	assert(!rnd_stack.empty());

	auto bl = std::make_unique<evtnodes::block_named>(obj);
	evtnodes::callable* ret = bl.get();
	result.nodes.emplace_back(std::move(bl));

	add_op_out(make_block_call(obj.name));

	result.roots.emplace_back(ret);
	ctx_stack.emplace_back(ret);
	rnd_stack.emplace_back(ret);

	for(auto& c:obj.content) boost::apply_visitor(*this, c.var);
	return ret;
}

void cppjinja::evt::tmpl_compiler::add_op_out(cppjinja::ast::op_out obj)
{
	assert(!rnd_stack.empty());
	auto out_node = std::make_unique<evtnodes::op_out>(std::move(obj));
	result.lrnd.emplace_back(rnd_stack.back(), out_node.get());
	result.nodes.emplace_back(std::move(out_node));
}

cppjinja::ast::op_out cppjinja::evt::tmpl_compiler::make_block_call(
        cppjinja::ast::string_t name) const
{
	ast::function_call bl_call;
	bl_call.ref = ast::var_name{name};
	ast::op_out out;
	out.value = bl_call;
	return out;
}

void cppjinja::evt::tmpl_compiler::operator()(ast::forward_ast<cppjinja::ast::block_macro>& obj)
{
	auto mcr = std::make_unique<evtnodes::block_macro>(std::move(obj));
	rnd_stack.emplace_back(mcr.get());
	ctx_stack.emplace_back(mcr.get());
	result.roots.emplace_back(mcr.get());
	result.nodes.emplace_back(std::move(mcr));

	for(auto& c:obj.get().content) boost::apply_visitor(*this, c.var);
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::op_set& obj)
{
	assert(1 <= ctx_stack.size());
	assert(dynamic_cast<evtnodes::callable*>(ctx_stack[0]) != nullptr);
	auto set_node = std::make_unique<evtnodes::op_set>(std::move(obj));
	result.lctx.emplace_back(ctx_stack.back(), set_node.get());
	result.nodes.emplace_back(std::move(set_node));
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::op_out& obj)
{
	add_op_out(std::move(obj));
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
