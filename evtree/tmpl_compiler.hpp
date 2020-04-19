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
#include "parser/ast/tmpls.hpp"
#include <iostream>

namespace cppjinja::evt {

struct edge {
	edge(node* p, node* c) : parent(p), child(c) {}
	node* parent;
	node* child;
};

struct compiled_tmpl final {
	compiled_tmpl(const compiled_tmpl&) =delete ;
	compiled_tmpl& operator = (const compiled_tmpl&) =delete ;
	compiled_tmpl(compiled_tmpl&&) =default ;
	compiled_tmpl& operator = (compiled_tmpl&&) =default ;
	compiled_tmpl() =default ;
	~compiled_tmpl() noexcept =default ;

	std::string tmpl_name;
	std::vector<std::unique_ptr<node>> nodes;
	std::vector<edge> lrnd;
	std::vector<edge> lctx;
};

class tmpl_compiler final {
	compiled_tmpl result;
	std::vector<node*> ctx_stack;
	std::vector<node*> rnd_stack;
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

bool operator == (const edge& l, const edge& r) noexcept ;

} // namespace cppjinja::evt
