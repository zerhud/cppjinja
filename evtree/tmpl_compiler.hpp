/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "node_tree.hpp"
#include "node.hpp"
#include "nodes/tmpl.hpp"
#include "nodes/callable.hpp"
#include "parser/ast/tmpls.hpp"
#include <iostream>

namespace cppjinja::evt {

typedef edge<node> node_edge;
typedef edge<evtnodes::callable> callable_edge;

struct compiled_tmpl final {
	compiled_tmpl(const compiled_tmpl&) =delete ;
	compiled_tmpl& operator = (const compiled_tmpl&) =delete ;
	compiled_tmpl(compiled_tmpl&&) =default ;
	compiled_tmpl& operator = (compiled_tmpl&&) =default ;
	compiled_tmpl() =default ;
	~compiled_tmpl() noexcept =default ;

	std::vector<ast::extend_st> extends;
	std::vector<std::unique_ptr<node>> nodes;
	std::vector<evtnodes::callable*> roots;
	node_tree<const node> render_tree;

	std::string tmpl_name() const ;
	evtnodes::tmpl* tmpl_node() ;
	const evtnodes::tmpl* tmpl_node() const ;
	evtnodes::callable* main_block();
};

class tmpl_compiler final {
	compiled_tmpl result;
	std::vector<node*> ctx_stack;
	std::vector<node*> rnd_stack;
	ast::tmpl cur_tmpl;

	template<typename N, typename ... Args>
	N* create_rendered_node(Args... args);
	template<typename N, typename ... Args>
	N* create_node(Args... args);

	void make_main_nodes();
	ast::block_named create_ast_main_block();
	evtnodes::callable* add_block(ast::block_named obj);
	template<typename Cnt, typename Node>
	void compile_content(Node* node, Cnt& cnt);

	bool can_render_in_place(const ast::block_named& obj) const ;
	void make_content_block(
	        evt::render_info ri, std::vector<ast::block_content> children);

	template<typename B>
	evt::render_info make_ri_for_if(const B& obj) const ;
	template<typename B>
	evt::render_info make_ri_for_else(const B& obj) const ;
public:
	compiled_tmpl operator()(ast::tmpl t) ;

	void operator()(ast::string_t& cnt);
	void operator()(ast::op_out& obj);
	void operator()(ast::op_comment& obj);
	void operator()(ast::op_set& obj);
	void operator()(ast::forward_ast<ast::block_raw>& obj);
	void operator()(ast::forward_ast<ast::block_if>& obj);
	void operator()(ast::forward_ast<ast::block_for>& obj);
	void operator()(ast::forward_ast<ast::block_macro>& obj);
	void operator()(ast::forward_ast<ast::block_named>& obj);
	void operator()(ast::forward_ast<ast::block_filtered>& obj);
	void operator()(ast::forward_ast<ast::block_set>& obj);
	void operator()(ast::forward_ast<ast::block_call>& obj);
};

template<typename L, typename R>
bool operator == (const edge<L>& l, const edge<R>& r) noexcept
{
	return l.parent == r.parent && l.child == r.child;
}

} // namespace cppjinja::evt
