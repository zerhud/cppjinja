/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evtree/declarations.hpp"
#include "parser/ast/expr.hpp"

namespace cppjinja::evt::expr_evals {

class cmp_check {
	ast::expr_ops::cmp_op op_;

	template<typename L, typename R>
	bool default_op(const L& l, const R& r) const
	{
		using ast::expr_ops::cmp_op;
		assert(op_!=cmp_op::nop);
		if(op_==cmp_op::eq) return l == r;
		if(op_==cmp_op::neq) return l != r;
		if(op_==cmp_op::less) return l < r;
		if(op_==cmp_op::more) return l > r;
		if(op_==cmp_op::less_eq) return l <= r;
		if(op_==cmp_op::more_eq) return l >= r;
		assert(false);
		return 0;
	}
	[[noreturn]] void throw_int_and_string_op() const ;
public:
	cmp_check(ast::expr_ops::cmp_op op);

	ast::expr_ops::term operator()(const bool& l, const bool& r) const ;
	ast::expr_ops::term operator()(const bool& l, const std::int64_t& r) const ;
	ast::expr_ops::term operator()(const bool& l, const double& r) const ;
	ast::expr_ops::term operator()(const bool& l, const ast::string_t& r) const ;

	ast::expr_ops::term operator()(const std::int64_t& l, const bool& r) const ;
	ast::expr_ops::term operator()(const std::int64_t& l, const std::int64_t& r) const ;
	ast::expr_ops::term operator()(const std::int64_t& l, const double& r) const ;
	ast::expr_ops::term operator()(const std::int64_t& l, const ast::string_t& r) const ;

	ast::expr_ops::term operator()(const double& l, const bool& r) const ;
	ast::expr_ops::term operator()(const double& l, const std::int64_t& r) const ;
	ast::expr_ops::term operator()(const double& l, const double& r) const ;
	ast::expr_ops::term operator()(const double& l, const ast::string_t& r) const ;

	ast::expr_ops::term operator()(const ast::string_t& l, const bool& r) const ;
	ast::expr_ops::term operator()(const ast::string_t& l, const std::int64_t& r) const ;
	ast::expr_ops::term operator()(const ast::string_t& l, const double& r) const ;
	ast::expr_ops::term operator()(const ast::string_t& l, const ast::string_t& r) const ;
};

} // namespace cppjinja::evt::expr_evals
