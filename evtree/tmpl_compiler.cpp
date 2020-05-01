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

namespace cppjinja::evt::details {

template<typename Cont, typename Value>
class push_pop_raii {
	Cont& cont;
public:
	push_pop_raii(Cont& c, Value* v) : cont(c)
	{
		cont.push_back(v);
	}
	~push_pop_raii() noexcept
	{
		cont.pop_back();
	}
};

} // namespace cppjinja::evt::details

cppjinja::evt::compiled_tmpl
cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::tmpl t)
{
	cur_tmpl = std::move(t);
	result.extends = std::move(cur_tmpl.extends);
	make_main_nodes();
	return std::move(result);
}

void cppjinja::evt::tmpl_compiler::make_main_nodes()
{
	create_node<evtnodes::tmpl>(cur_tmpl);
	details::push_pop_raii rnd_raii(rnd_stack, result.tmpl_node());
	add_block(create_ast_main_block());
	result.render_tree.add_root(result.tmpl_node());
	result.render_tree.add_child(result.tmpl_node(), result.main_block());
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
	evtnodes::callable* ret = create_node<evtnodes::block_named>(obj);
	result.render_tree.add_root(ret);
	compile_content(ret, obj.content);
	return result.roots.emplace_back(ret);
}

template<typename Cnt, typename Node>
void cppjinja::evt::tmpl_compiler::compile_content(Node* node, Cnt& cnt)
{
	details::push_pop_raii ctx_raii(ctx_stack, node);
	details::push_pop_raii rnd_raii(rnd_stack, node);
	for(auto& c:cnt) boost::apply_visitor(*this, c.var);
}

template<typename N, typename ... Args>
N* cppjinja::evt::tmpl_compiler::create_rendered_node(Args ... args)
{
	assert(!rnd_stack.empty());
	N* ret = create_node<N>(std::forward<Args>(args)...);
	result.render_tree.add_child(rnd_stack.back(), ret);
	return ret;
}

template<typename N, typename ... Args>
N* cppjinja::evt::tmpl_compiler::create_node(Args ... args)
{
	auto node = std::make_unique<N>(std::forward<Args>(args)...);
	N* ret = node.get();
	result.nodes.emplace_back(std::move(node));
	return ret;
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::string_t& cnt)
{
	assert(!ctx_stack.empty());
	assert(!rnd_stack.empty());
	create_rendered_node<evtnodes::content>(std::move(cnt));
}

void cppjinja::evt::tmpl_compiler::operator()(ast::forward_ast<ast::block_named>& obj)
{
	const bool render_in_place = can_render_in_place(obj.get());
	auto bl = add_block(obj.get());
	if(render_in_place) result.render_tree.add_child(rnd_stack.back(), bl);
}

bool cppjinja::evt::tmpl_compiler::can_render_in_place(
        const cppjinja::ast::block_named& obj) const
{
	if(obj.params.empty()) return true;
	for(auto&p:obj.params) if(!p.value.has_value()) return false;
	return true;
}

void cppjinja::evt::tmpl_compiler::operator()(
        ast::forward_ast<cppjinja::ast::block_if>& obj)
{
	auto if_node = create_rendered_node<evtnodes::block_if>(obj.get());
	details::push_pop_raii rnd_raii(rnd_stack, if_node);
	make_content_block(make_ri_for_if(obj.get()), std::move(obj.get().content));
	if(obj.get().else_block)
		make_content_block(
		            make_ri_for_else(obj.get()),
		            std::move(obj.get().else_block->content));
}

cppjinja::evt::render_info cppjinja::evt::tmpl_compiler::make_ri_for_if(const cppjinja::ast::block_if& obj) const
{
	return evt::render_info{
		obj.left_close.trim,
		obj.else_block ?
		            obj.else_block->left_open.trim :
		            obj.right_open.trim};
}

cppjinja::evt::render_info cppjinja::evt::tmpl_compiler::make_ri_for_else(const cppjinja::ast::block_if& obj) const
{
	assert(obj.else_block.has_value());
	return evt::render_info{obj.else_block->left_close.trim, obj.right_open.trim};
}

void cppjinja::evt::tmpl_compiler::make_content_block(
          cppjinja::evt::render_info ri
        , std::vector<cppjinja::ast::block_content> children)
{
	assert(2 <= rnd_stack.size());
	details::push_pop_raii rnd_raii( rnd_stack,
	            create_rendered_node<evtnodes::content_block>(ri) );
	for(auto& c:children) boost::apply_visitor(*this, c.var);
}

void cppjinja::evt::tmpl_compiler::operator()(
            ast::forward_ast<cppjinja::ast::block_raw>& obj)
{
	assert(!ctx_stack.empty());
	assert(!rnd_stack.empty());
	create_rendered_node<evtnodes::content>(std::move(obj.get().value));
}

void cppjinja::evt::tmpl_compiler::operator()(
        ast::forward_ast<cppjinja::ast::block_macro>& obj)
{
	auto mcr = create_node<evtnodes::block_macro>(obj.get());
	result.render_tree.add_root(mcr);
	compile_content(result.roots.emplace_back(mcr), obj.get().content);
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::op_set& obj)
{
	assert(!rnd_stack.empty());
	assert(1 <= ctx_stack.size());
	assert(dynamic_cast<evtnodes::callable*>(ctx_stack[0]) != nullptr);
	create_rendered_node<evtnodes::op_set>(std::move(obj));
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::op_out& obj)
{
	create_rendered_node<evtnodes::op_out>(std::move(obj));
}

std::string cppjinja::evt::compiled_tmpl::tmpl_name() const
{
	return tmpl_node()->name();
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
