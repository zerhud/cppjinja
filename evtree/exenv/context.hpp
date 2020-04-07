/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../evtree.hpp"

namespace cppjinja::evt {

class context_new final {
	struct frame
	{
		const node* maker;
		std::vector<const evtnodes::callable*> injections;
		std::vector<const node*> node_stack;
	};
	std::vector<frame> ctx;

	void require_not_empty() const ;
	std::optional<ast::value_term> search_in_setts(
	        const cppjinja::ast::var_name& var) const;
	std::optional<ast::value_term> search_in_params(
	        const cppjinja::ast::var_name& var) const;
public:
	context_new();

	void current_node(const node* n) ;
	const node* current_node(std::size_t ind=0) const ;

	void pop(const node* n);
	void push(const node* n);
	const node* maker() const ;

	void inject(const evtnodes::callable* n);
	void takeout(const evtnodes::callable* n);

	std::optional<ast::value_term> solve_var(
	        const cppjinja::ast::var_name& var) const;
};

class raii_push_ctx {
	context_new* ctx;
	const node* maker;
public:
	raii_push_ctx(const node* n, context_new* c);
	~raii_push_ctx() ;
};

} // namespace cppjinja::evt
