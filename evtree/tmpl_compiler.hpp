/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "evtree/node.hpp"
#include "nodes/tmpl.hpp"
#include "nodes/callable.hpp"
#include "parser/ast/tmpls.hpp"
#include <iostream>

namespace cppjinja::evt {

template<typename Node>
struct edge {
	edge(Node* p, Node* c) : parent(p), child(c) {}
	Node* parent;
	Node* child;
};

typedef edge<node> node_edge;
typedef edge<evtnodes::callable> callable_edge;

struct compiled_tmpl final {
	compiled_tmpl(const compiled_tmpl&) =delete ;
	compiled_tmpl& operator = (const compiled_tmpl&) =delete ;
	compiled_tmpl(compiled_tmpl&&) =default ;
	compiled_tmpl& operator = (compiled_tmpl&&) =default ;
	compiled_tmpl() =default ;
	~compiled_tmpl() noexcept =default ;

	std::string tmpl_name;
	std::vector<std::unique_ptr<node>> nodes;
	std::vector<evtnodes::callable*> roots;
	std::vector<node_edge> lrnd;
	std::vector<node_edge> lctx;

	evtnodes::tmpl* tmpl_node() ;
	evtnodes::callable* main_block();
};

class tmpl_compiler final {
	compiled_tmpl result;
	std::vector<node*> ctx_stack;
	std::vector<node*> rnd_stack;
	ast::tmpl cur_tmpl;

	void compile_template_content();
	void make_main_nodes();
	evtnodes::callable* add_block(ast::block_named obj);
public:
	compiled_tmpl operator()(ast::tmpl t) ;

	template<typename T>
	void operator()(T& obj) {
		(void)obj;
		std::cout << "here " << std::endl;
	}
	void operator()(ast::string_t& cnt);
	void operator()(ast::forward_ast<ast::block_named>& obj);
};

template<typename L, typename R>
bool operator == (const edge<L>& l, const edge<R>& r) noexcept
{
	return l.parent == r.parent && l.child == r.child;
}

} // namespace cppjinja::evt
