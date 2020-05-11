/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "logic_check.hpp"



void cppjinja::evt::expr_evals::logic_check::throw_int_and_string_op() const
{
	throw std::runtime_error("cannot compare string and number");
}

cppjinja::evt::expr_evals::logic_check::logic_check(cppjinja::ast::expr_ops::logic_op op)
    : op_(op)
{
}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const bool& l, const bool& r) const
{
	return default_op(l,r);
}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const bool& l, const std::int64_t& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const bool& l, const double& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const bool&, const ast::string_t&) const
{throw_int_and_string_op();}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const std::int64_t& l, const bool& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const std::int64_t& l, const std::int64_t& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const std::int64_t& l, const double& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const std::int64_t&, const ast::string_t&) const
{throw_int_and_string_op();}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const double& l, const bool& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const double& l, const std::int64_t& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const double& l, const double& r) const
{return default_op(l,r);}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const double&, const ast::string_t&) const
{throw_int_and_string_op();}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const ast::string_t&, const bool&) const
{throw_int_and_string_op();}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const ast::string_t&, const std::int64_t&) const
{throw_int_and_string_op();}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const ast::string_t&, const double&) const
{throw_int_and_string_op();}
cppjinja::ast::expr_ops::term cppjinja::evt::expr_evals::logic_check::operator()(const ast::string_t& l, const ast::string_t& r) const
{return default_op(l.empty(),r.empty()) || default_op(!l.empty(),!r.empty());}
