/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/expr.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::list, items)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::tuple, items)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::dict, items)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::dict_item, name, value)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::eq_assign, names, value)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::in_assign, names, value)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::math, left, op, right)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::concat, left, right)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::in_check, term, object)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::cmp_check, left, op, right)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::logic_check, left, op, right)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::negate, arg)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::fnc_call, ref, args)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::filter_call, ref, args)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::filter, base, filters)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::single_var_name, name)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::point, left, right)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::expr_ops::op_if, term, cond, alternative)
