/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <list>

#include "context.hpp"
#include "result_formatter.hpp"
#include "context_objects/tree.hpp"

namespace cppjinja::evt {

class context_impl final : public context {
	struct frame
	{
		const node* maker;
		std::vector<const node*> node_stack;
		std::stringstream out;
		context_objects::tree ns;
	};
	std::list<frame> ctx;

	void require_not_empty() const ;
public:
	context_impl();
	~context_impl() noexcept ;

	void current_node(const node* n) override ;
	const node* nth_node_on_stack(std::size_t ind) const override ;

	void pop(const node* n)override ;
	void push(const node* n)override ;
	const node* maker() const override ;

	std::ostream& out() override ;
	std::string result() const override ;

	context_object& cur_namespace() override ;
};

} // namespace cppjinja::evt
