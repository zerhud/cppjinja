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
	std::vector<const node*> cur_nodes;

	struct frame
	{
		const node* maker;
		std::vector<const evtnodes::callable*> injections;
	};
	std::vector<frame> ctx;

	void require_not_empty() const ;
public:
	context_new();

	void current_node(const node* n) ;
	const node* current_node(std::size_t ind=0) const ;

	void pop(const node* n);
	void push(const node* n);
	const node* maker() const ;

	void inject(const evtnodes::callable* n);
	void takeout(const evtnodes::callable* n);

	east::value_term solve_value(const cppjinja::ast::value_term& val) const ;
};

} // namespace cppjinja::evt
