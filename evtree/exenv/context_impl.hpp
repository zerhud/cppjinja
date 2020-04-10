/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>

#include "context.hpp"

namespace cppjinja::evt {

class context_impl final : public context {
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
public:
	context_impl();

	void current_node(const node* n) ;
	const node* nth_node_on_stack(std::size_t ind) const ;

	void pop(const node* n);
	void push(const node* n);
	const node* maker() const ;

	void inject(const evtnodes::callable* n);
	void takeout(const evtnodes::callable* n);
	std::vector<const evtnodes::callable*> injections() const ;

	std::optional<ast::value_term> solve_var(
	        const cppjinja::ast::var_name& var) const;
};

} // namespace cppjinja::evt
