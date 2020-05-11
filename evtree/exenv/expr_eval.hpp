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

	east::value_term cvt(const ast::expr_ops::term& v) const ;
	ast::string_t to_str(const ast::expr_ops::term& v) const ;
	ast::string_t to_str(const east::value_term& v) const ;
	void solve_point_arg(ast::expr_ops::point_element& left) const ;
	void solve_point_arg(ast::expr_ops::single_var_name& left) const ;
	void solve_point_arg(ast::expr_ops::expr& left) const ;
	void solve_point_arg(ast::expr_ops::point& left) const ;
public:
	typedef ast::expr_ops::term eval_type;

	expr_eval(const exenv* e);

	std::shared_ptr<evt::context_object> operator () (ast::expr_ops::expr t) const ;

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
	std::string replace_back(std::stringstream src, char nback) const ;
	std::ostream& print_quoted(std::stringstream& out, ast::expr_ops::term v) const ;
	std::ostream& print_quoted(std::stringstream& out, east::value_term v) const ;
};

} // namespace cppjinja::evt
