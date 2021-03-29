/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evtree/exenv.hpp"
#include "eval/eval.hpp"

#include "context_impl.hpp"
#include "callstack_impl.hpp"
#include "context_objects/tree.hpp"
#include "context_objects/user_data.hpp"
#include "context_objects/builtins.hpp"

namespace cppjinja::evt {

class exenv_impl final : public exenv {
	const evtree* compiled_template;
	context_objects::user_data given_data;
	context_impl exectx;
	callstack_impl execalls;
	std::optional<render_info> cur_rinfo;
	context_objects::tree global_namespace;
	context_objects::builtins builtins;
	result_formatter rfmt;
public:
	exenv_impl(const data_provider* prov, const evtree* tmpl);

	const evtree& tmpl() const override ;
	std::pmr::vector<const node*> children(const node* selected ) const override ;
	std::pmr::vector<const evtnodes::callable*> roots(
	        const evtnodes::tmpl* tmpl) const override ;

	std::shared_ptr<std::pmr::memory_resource> storage() const override ;

	const data_provider* data() const override ;
	std::ostream& out() override ;
	absd::data result() const override;

	context& ctx() override ;
	const context& ctx() const override ;
	void current_node(const node* n) override ;

	context_object& locals() override ;
	context_object& globals() override ;
	const context_objects::queue params() const override ;
	const context_object& user_data() const override ;
	const context_objects::queue all_ctx() const override ;

	callstack& calls() override ;

	result_formatter& render_format() override;
	std::optional<render_info> rinfo() const override ;
	void rinfo(std::optional<render_info> ri) override ;
};

} // namespace cppjinja::evt
