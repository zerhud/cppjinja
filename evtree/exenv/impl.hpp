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

namespace cppjinja::evt {

class exenv_impl final : public exenv {
	const evtree* compiled_template;
	const data_provider* user_data;
	context_impl exectx;
	callstack_impl execalls;
	std::optional<render_info> cur_rinfo;
public:
	exenv_impl(const data_provider* prov, const evtree* tmpl);

	const evtree& tmpl() const override ;
	std::vector<const node*> children(const node* selected ) const override ;
	const evtnodes::callable* search_callable(std::string_view name) const override ;

	const data_provider* data() const override ;
	std::ostream& out() override ;
	east::string_t result() const override;

	context& ctx() override ;
	const context& ctx() const override ;
	void current_node(const node* n) override ;

	std::optional<ast::value_term> solve_var(
	        const ast::var_name& var) const override ;
	std::optional<ast::value_term> solve_call(
	        const ast::function_call& call) const override ;


	callstack& calls() override ;
	const callstack& calls() const override ;

	std::optional<render_info> rinfo() const override ;
	void rinfo(std::optional<render_info> ri) override ;
};

} // namespace cppjinja::evt
