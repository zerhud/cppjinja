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
public:
	typedef absd::data eval_type;
private:
	const exenv* env;
	ast::expr_ops::expr src_operation;

	static evt::context_object_ptr make_value_object(eval_type v) ;

	absd::data cvt(const ast::expr_ops::term& v) const ;
	std::pmr::string to_str(const absd::data& v) const ;
	bool to_bool(const absd::data& v) const ;
	ast::expr_ops::term to_term(const absd::data& d) const ;

	evt::context_object_ptr solve_point_arg(const evt::context_object& left, ast::expr_ops::point_element& expr) const ;
	evt::context_object_ptr solve_point_arg(const evt::context_object& left, ast::expr_ops::single_var_name& epxr) const ;
	evt::context_object_ptr solve_point_arg(const evt::context_object& left, ast::expr_ops::expr& expr) const ;
	evt::context_object_ptr solve_point_arg(const evt::context_object& left, ast::expr_ops::point& expr) const ;
	context_object::function_parameter make_param(ast::expr_ops::expr& pexpr) const ;
	std::optional<east::string_t> make_param_name(ast::expr_ops::lvalue& name) const ;
	eval_type filter_content(eval_type base, ast::expr_ops::filter_call& call) const ;
	evt::context_object_ptr solve_ref(ast::expr_ops::lvalue& ref) const ;
	absd::data perform_test(ast::expr_ops::cmp_check& t) const ;

	template<typename T>
	absd::data create_data(T&& arg) const;
public:
	expr_eval(const exenv* e);

	eval_type operator () (ast::expr_ops::expr t) const ;
	eval_type operator () (ast::expr_ops::lvalue ref) const ;
	bool operator() (ast::expr_ops::expr_bool e) const ;

	eval_type operator () (ast::expr_ops::term& t) const ;
	eval_type operator () (ast::expr_ops::single_var_name& t) const ;
	eval_type operator () (ast::expr_ops::list& t) const ;
	eval_type operator () (ast::expr_ops::tuple& t) const ;
	eval_type operator () (ast::expr_ops::dict& t) const ;
	eval_type operator () (ast::expr_ops::eq_assign& t) const ;
	eval_type operator () (ast::expr_ops::in_assign& t) const ;
	eval_type operator () (ast::expr_ops::math& t) const ;
	eval_type operator () (ast::expr_ops::concat& t) const ;
	eval_type operator () (ast::expr_ops::in_check& t) const ;
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

	template<typename T>
	eval_type eval(ast::forward_ast<T>& v) const
	{
		return boost::apply_visitor(*this, v.get().var);
	}
};

} // namespace cppjinja::evt
