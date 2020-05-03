/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include "../context_object.hpp"
#include "evtree/nodes/callable.hpp"

namespace cppjinja::evt::context_objects {

class callable_node : public context_object {
	evt::exenv* env;
	evtnodes::callable* node;
public:
	callable_node(evt::exenv* e, evtnodes::callable* n);
	~callable_node() noexcept override ;

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
	east::value_term call(std::vector<east::function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
