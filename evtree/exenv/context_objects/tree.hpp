/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <string>
#include "../context_object.hpp"

namespace cppjinja::evt::context_objects {

class tree : public context_object {
	std::pmr::map<east::string_t, std::shared_ptr<context_object>> children;
	std::ostream& print_solved_value(std::ostream& out, const context_object& obj) const ;
	std::ostream& render_children_with_comma(std::ostream& out) const ;
protected:
	void add_child(east::string_t n, std::shared_ptr<context_object> child);
public:
	tree();
	~tree() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::string_t n) const override ;
	absd::data solve() const override ;
	std::shared_ptr<context_object> call(
	        std::pmr::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
