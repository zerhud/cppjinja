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
#include "context_objects/queue.hpp"

namespace cppjinja::evt {

class context_impl final : public context {
	struct frame
	{
		const node* maker;
		std::vector<const node*> node_stack;
		std::stringstream out;
		context_objects::tree ns;
		std::optional<context_objects::queue> shadow;
	};
	std::list<frame> ctx;

	void require_not_empty() const ;
	frame& last_shadow() ;
	const frame& last_shadow() const ;
	void create_shadow();
public:
	context_impl();
	~context_impl() noexcept ;

	void current_node(const node* n) override ;
	const node* nth_node_on_stack(std::size_t ind) const override ;

	void pop(const node* n)override ;
	void push(const node* n)override ;
	void push_shadow(const node* n) override ;
	const node* maker() const override ;

	std::ostream& out() override ;
	std::pmr::string result() const override ;

	context_object& cur_namespace() override ;
};

} // namespace cppjinja::evt
