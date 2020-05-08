/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval.hpp"
#include <boost/variant.hpp>
#include <iostream>

namespace cppjinja::evt::expr_evals {

class expr_eval_to_term : public expr_eval_to_any<ast::expr_ops::term>
{
	using cover_elements = std::tuple<ast::expr_ops::term, ast::expr_ops::math>;

	template<typename Left, typename Right>
	result_t math_to_term(Left&& l, Right&& r, ast::expr_ops::math_op op)
	{
		using ast::expr_ops::math_op;
		if(op == math_op::pls) return result_t{l + r};
		throw  std::logic_error("this operation are not supported");
	}

	template <typename T1, typename... T2>
	static constexpr bool tuple_contains(std::tuple<T2...>) {
		return std::disjunction_v<std::is_same<T1, T2>...>;
	}
public:
	template<typename Val>
	std::enable_if_t<tuple_contains(Val{}, cover_elements{}),ret_t>
	operator()(Val&& v)
	{
		std::cout << "=== " << __LINE__ << std::endl;
		std::cout << "\t" << typeid(Val).name() << std::endl;
		throw std::runtime_error("cannot evaluate to term");
	}

	ret_t operator()(const ast::expr_ops::term& v)
	{
		return v;
	}

	ret_t operator()(const ast::expr_ops::math& m)
	{
		std::cout << "=== " << __LINE__ << std::endl;
		ret_t left = boost::apply_visitor(*this, m.left.get().var);
		ret_t right = boost::apply_visitor(*this, m.right.get().var);
		auto math = [this,&m](auto&& l, auto&& r){
			constexpr bool lisstr = std::is_same_v<std::decay_t<decltype(l)>, ast::string_t>;
			constexpr bool risstr = std::is_same_v<std::decay_t<decltype(r)>, ast::string_t>;
			if constexpr (lisstr && risstr) return math_to_term(l, r, m.op);
			else if constexpr(lisstr) return math_to_term(std::stod(l), r, m.op);
			else if constexpr(risstr) return math_to_term(l, std::stod(r), m.op);
			else return math_to_term(l, r, m.op);
		};
		return boost::apply_visitor(math, *left, *right);
	}
};

} // namespace cppjinja::evt::expr_evals
