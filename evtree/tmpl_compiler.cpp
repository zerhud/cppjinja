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
#include "nodes/block_if.hpp"
#include "nodes/content_block.hpp"

typedef std::unique_ptr<cppjinja::evt::node> node_ptr;
typedef std::vector<node_ptr> vec_type;

cppjinja::evt::compiled_tmpl
cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::tmpl t)
{
	cur_tmpl = std::move(t);
	result.tmpl_name = cur_tmpl.name;
	result.extends = std::move(cur_tmpl.extends);
	make_main_nodes();
	return std::move(result);
}

void cppjinja::evt::tmpl_compiler::make_main_nodes()
{
	result.nodes.emplace_back(std::make_unique<evtnodes::tmpl>(cur_tmpl));
	rnd_stack.emplace_back(result.tmpl_node());
	add_block(create_ast_main_block());
	result.lrnd.emplace_back(result.tmpl_node(), result.main_block());
}

cppjinja::ast::block_named cppjinja::evt::tmpl_compiler::create_ast_main_block()
{
	ast::block_named main_bl;
	main_bl.content = std::move(cur_tmpl.content);
	return main_bl;
}

cppjinja::evtnodes::callable*
cppjinja::evt::tmpl_compiler::add_block(ast::block_named obj)
{
	assert(!rnd_stack.empty());

	auto bl = std::make_unique<evtnodes::block_named>(obj);
	evtnodes::callable* ret = bl.get();
	result.nodes.emplace_back(std::move(bl));
	result.lctx.emplace_back(result.tmpl_node(), ret);

	result.roots.emplace_back(ret);
	ctx_stack.emplace_back(ret);
	rnd_stack.emplace_back(ret);

	for(auto& c:obj.content) boost::apply_visitor(*this, c.var);
	rnd_stack.pop_back();
	return ret;
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

void cppjinja::evt::tmpl_compiler::add_op_out(cppjinja::ast::op_out obj)
{
	create_node<evtnodes::op_out>(std::move(obj));
}

template<typename N, typename ... Args>
N* cppjinja::evt::tmpl_compiler::create_node(Args ... args)
{
	assert(!rnd_stack.empty());
	auto node = std::make_unique<N>(std::forward<Args>(args)...);
	N* ret = node.get();
	result.nodes.emplace_back(std::move(node));
	result.lrnd.emplace_back(rnd_stack.back(), ret);
	return ret;
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::string_t& cnt)
{
	assert(!ctx_stack.empty());
	assert(!rnd_stack.empty());
	create_node<evtnodes::content>(std::move(cnt));
}

void cppjinja::evt::tmpl_compiler::operator()(ast::forward_ast<ast::block_named>& obj)
{
	if(obj.get().params.empty())
		add_op_out(make_block_call(obj.get().name));
	add_block(obj.get());
}

void cppjinja::evt::tmpl_compiler::operator()(
        ast::forward_ast<cppjinja::ast::block_if>& obj)
{
	assert(!rnd_stack.empty());
	rnd_stack.emplace_back(create_node<evtnodes::block_if>(obj));

	evt::render_info ri{
		obj.get().left_close.trim,
		obj.get().else_block ?
		            obj.get().else_block->left_open.trim :
		            obj.get().right_open.trim};
	make_content_block(ri, std::move(obj.get().content));

	if(!obj.get().else_block){
		rnd_stack.pop_back();
		return;
	}

	ri.trim_left = obj.get().else_block->left_close.trim;
	ri.trim_right = obj.get().right_open.trim;
	make_content_block(ri, std::move(obj.get().else_block->content));

	rnd_stack.pop_back();
}

void cppjinja::evt::tmpl_compiler::make_content_block(
          cppjinja::evt::render_info ri
        , std::vector<cppjinja::ast::block_content> children)
{
	assert(2 <= rnd_stack.size());
	rnd_stack.emplace_back(create_node<evtnodes::content_block>(ri, ""));
	for(auto& c:children) boost::apply_visitor(*this, c.var);
	rnd_stack.pop_back();
}

void cppjinja::evt::tmpl_compiler::operator()(
            ast::forward_ast<cppjinja::ast::block_raw>& obj)
{
	assert(!ctx_stack.empty());
	assert(!rnd_stack.empty());
	create_node<evtnodes::content>(std::move(obj.get().value));
}

void cppjinja::evt::tmpl_compiler::operator()(
        ast::forward_ast<cppjinja::ast::block_macro>& obj)
{
	auto mcr = std::make_unique<evtnodes::block_macro>(std::move(obj));
	result.lctx.emplace_back(result.tmpl_node(), mcr.get());
	rnd_stack.emplace_back(mcr.get());
	ctx_stack.emplace_back(mcr.get());
	result.roots.emplace_back(mcr.get());
	result.nodes.emplace_back(std::move(mcr));

	for(auto& c:obj.get().content) boost::apply_visitor(*this, c.var);
	rnd_stack.pop_back();
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::op_set& obj)
{
	assert(!rnd_stack.empty());
	assert(1 <= ctx_stack.size());
	assert(dynamic_cast<evtnodes::callable*>(ctx_stack[0]) != nullptr);
	auto* set_node = create_node<evtnodes::op_set>(std::move(obj));
	result.lctx.emplace_back(ctx_stack.back(), set_node);
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

const cppjinja::evtnodes::tmpl* cppjinja::evt::compiled_tmpl::tmpl_node() const
{
	return const_cast<compiled_tmpl*>(this)->tmpl_node();
}

cppjinja::evtnodes::callable* cppjinja::evt::compiled_tmpl::main_block()
{
	if(nodes.size() < 2) return nullptr;
	auto ret = dynamic_cast<evtnodes::callable*>(nodes[1].get());
	assert(ret);
	return ret;
}
