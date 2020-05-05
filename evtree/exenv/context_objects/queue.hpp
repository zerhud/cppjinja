/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../context_object.hpp"

namespace cppjinja::evt::context_objects {

class queue : public context_object {
	std::vector<context_object*> content;
	std::vector<const context_object*> const_content;
public:
	queue();
	queue(std::initializer_list<queue> cnt);
	queue(std::initializer_list<context_object*> cnt);
	queue(std::initializer_list<const context_object*> cnt);
	queue(std::vector<context_object*> cnt);
	queue(std::vector<const context_object*> cnt);

	~queue() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
	std::shared_ptr<context_object> call(std::vector<east::function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
