/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/eval.hpp"

namespace cppjinja {

class evtree;

namespace evtnodes {
class callable;
} // namespace evtnodes

namespace evt {

/// render unit, node in render tree.
/// contains infromation for render.
class node;
/// help iformation for render node in context
struct render_info
{
	bool trim_left:4;
	bool trim_right:4;
};

class context {
	struct callstack_frame
	{
		const node* caller;
		std::vector<ast::function_call_parameter> params;
	};

	struct context_frame
	{
		const node* maker;
		std::vector<const node*> ctx;
	};

	std::vector<context_frame> ctx;
	std::vector<callstack_frame> callstack;
	std::vector<const node*> cur_nodes;
	std::optional<render_info> cur_info;

	const data_provider* prov;
	const evtree* tree_;

	std::ostream& outstream;

	const evtnodes::callable* cb_ctx_maker()const;
	const node* search_in_tree(const ast::var_name& n)const;
public:
	context(const data_provider* p, const evtree* t, std::ostream& out);

	std::ostream& out() ;

	const data_provider& data() const ;
	const evtree& tree() const ;

	east::value_term concreate_value(const ast::value_term& val) ;
	east::value_term filter(
	        const east::value_term& base,
	        const ast::value_term& val);

	void pop_context(const node* m);
	void push_context(const node* m);
	void add_context(const node* n);
	const node* ctx_maker() const ;

	void current_node(const node* n);
	const node* current_node(std::size_t ind=0) const ;

	std::optional<render_info> cur_render_info() const ;
	void cur_render_info(std::optional<render_info> ni) ;

	const node* caller() const ;
	void pop_callstack(const node* n);
	void push_callstack(const node* n);
	std::vector<ast::function_call_parameter> call_params() const ;
	void call_params(std::vector<ast::function_call_parameter> params);
};

}} // namespace cppjinja::evtree
