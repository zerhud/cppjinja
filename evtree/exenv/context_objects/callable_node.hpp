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
	const evtnodes::callable* node;
	const evtnodes::tmpl* cross_tmpl_call=nullptr;
public:
	callable_node(evt::exenv* e, const evtnodes::callable* n);
	callable_node(exenv *e, const evtnodes::callable *n, const evtnodes::tmpl* ctc);
	~callable_node() noexcept override ;

	bool is_it(const evtnodes::callable* n) const ;
	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::string_t n) const override ;
	absd::data solve() const override ;
	std::shared_ptr<context_object> call(
	        std::pmr::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evt::context_objects
