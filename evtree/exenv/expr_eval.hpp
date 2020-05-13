/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/ast.hpp"
#include "parser/ast/expr.hpp"
#include "evtree/declarations.hpp"
#include "evtree/exenv/context_object.hpp"

namespace cppjinja::evt {


class expr_eval final {
	const exenv* env;
	ast::expr_ops::expr src_operation;
	mutable std::shared_ptr<evt::context_object> result;

	cppjinja::json cvt(const ast::expr_ops::term& v) const ;
	ast::string_t to_str(const cppjinja::json& v) const ;
	ast::expr_ops::term to_term(const json& j) const ;

	void solve_point_arg(ast::expr_ops::point_element& left) const ;
	void solve_point_arg(ast::expr_ops::single_var_name& left) const ;
	void solve_point_arg(ast::expr_ops::expr& left) const ;
	void solve_point_arg(ast::expr_ops::point& left) const ;
	east::function_parameter make_param(ast::expr_ops::expr& pexpr) const ;
	std::optional<east::string_t> make_param_name(ast::expr_ops::lvalue& name) const ;
	void filter_content(ast::expr_ops::filter_call& call) const ;
	std::shared_ptr<evt::context_object> solve_ref(ast::expr_ops::lvalue& ref) const ;
public:
	typedef cppjinja::json eval_type;

	expr_eval(const exenv* e);

	std::shared_ptr<evt::context_object> operator () (ast::expr_ops::expr t) const ;
	std::shared_ptr<evt::context_object> operator () (ast::expr_ops::lvalue ref) const ;

	eval_type operator () (ast::expr_ops::term& t) const ;
	eval_type operator () (ast::expr_ops::single_var_name& t) const ;
	eval_type operator () (ast::expr_ops::list& t) const ;
	eval_type operator () (ast::expr_ops::tuple& t) const ;
	eval_type operator () (ast::expr_ops::dict& t) const ;
	eval_type operator () (ast::expr_ops::eq_assign& t) const ;
	eval_type operator () (ast::expr_ops::in_assign& t) const ;
	eval_type operator () (ast::expr_ops::math& t) const ;
	eval_type operator () (ast::expr_ops::concat& t) const ;
	eval_type operator () (ast::expr_ops::cmp_check& t) const ;
	eval_type operator () (ast::expr_ops::logic_check& t) const ;
	eval_type operator () (ast::expr_ops::negate& t) const ;
	eval_type operator () (ast::expr_ops::fnc_call& t) const ;
	eval_type operator () (ast::expr_ops::filter& t) const ;
	eval_type operator () (ast::expr_ops::point& t) const ;
	eval_type operator () (ast::expr_ops::op_if& t) const ;
private:
	template<typename T>
	eval_type visit(ast::forward_ast<T>& v) const
	{
		return boost::apply_visitor(*this, v.get().var);
	}
};

} // namespace cppjinja::evt
