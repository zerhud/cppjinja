/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "math.hpp"

using cppjinja::ast::expr_ops::term;

void cppjinja::evt::expr_evals::math::throw_int_and_string_op() const
{
	throw std::runtime_error("cannot evaluate a math operation on integer and string");
}

cppjinja::evt::expr_evals::math::math(cppjinja::ast::expr_ops::math_op op)
    : op_(op)
{
}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const bool& l, const bool& r) const
{return term{default_op<std::int64_t>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const bool& l, const std::int64_t& r) const
{return term{default_op<std::int64_t>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const bool& l, const double& r) const
{return term{default_op<double>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const bool& , const ast::string_t& ) const
{ throw_int_and_string_op(); }

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const std::int64_t& l, const bool& r) const
{return term{default_op<std::int64_t>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const std::int64_t& l, const std::int64_t& r) const
{
	return term{default_op<std::int64_t>(l, r)};
}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const std::int64_t& l, const double& r) const
{
	return term{default_op<double>((double)l, r)};
}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const std::int64_t& , const ast::string_t& ) const
{ throw_int_and_string_op(); }

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const double& l, const bool& r) const
{return term{default_op<double>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const double& l, const std::int64_t& r) const
{return term{default_op<double>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const double& l, const double& r) const
{return term{default_op<double>(l, r)};}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const double& , const ast::string_t& ) const
{ throw_int_and_string_op(); }

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const ast::string_t& , const bool& ) const
{ throw_int_and_string_op(); }
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const ast::string_t& , const std::int64_t& ) const
{ throw_int_and_string_op(); }
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const ast::string_t& , const double& ) const
{ throw_int_and_string_op(); }
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::math::operator()(const ast::string_t& l, const ast::string_t& r) const
{
	if(op_ == ast::expr_ops::math_op::pls) return term{l + r};
	throw std::runtime_error("for strings only + operation is avaible");
}
