/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/ast.hpp"
#include "parser/ast/common.hpp"

namespace cppjinja::evtnodes {

//TODO: add tests
class binary_op_helper {
	ast::comparator cmp_;
	template<typename L>
	bool default_in(const L& l, const east::array_v& r) const
	{
		using ast::comparator;
		assert(cmp_ == comparator::in);

		binary_op_helper cmp(comparator::eq);
		auto do_cmp = [cmp,&l](auto& r){return cmp(l,r);};
		for(auto& i:r.items)
			if(std::visit(do_cmp, (east::value_term_var&)(*i)))
				return true;
		return false;
	}

	template<typename L, typename R>
	bool default_cmp(const L& l, const R& r) const
	{
		using ast::comparator;
		assert(cmp_!=comparator::no);
		if(cmp_==comparator::eq) return l == r;
		if(cmp_==comparator::in) return l == r;
		if(cmp_==comparator::neq) return l != r;
		if(cmp_==comparator::less) return l < r;
		if(cmp_==comparator::more) return l > r;
		if(cmp_==comparator::less_eq) return l <= r;
		if(cmp_==comparator::more_eq) return l >= r;
		assert(false);
		return false;
	}
public:
	binary_op_helper(ast::comparator op);

	bool operator()(const int& v) const =delete ;

	bool operator()(const bool& v) const ;
	bool operator()(const double& v) const ;
	bool operator()(const east::string_t& v) const ;
	bool operator()(const east::array_v& v) const ;
	bool operator()(const east::map_v& v) const ;

	bool operator()(const bool& l, const bool& r) const ;
	bool operator()(const bool& l, const double& r) const ;
	bool operator()(const bool& l, const east::string_t& r) const ;
	bool operator()(const bool& l, const east::array_v& r) const ;
	bool operator()(const bool& l, const east::map_v& r) const ;

	bool operator()(const double& l, const bool& r) const ;
	bool operator()(const double& l, const double& r) const ;
	bool operator()(const double& l, const east::string_t& r) const ;
	bool operator()(const double& l, const east::array_v& r) const ;
	bool operator()(const double& l, const east::map_v& r) const ;

	bool operator()(const east::string_t& l, const bool& r) const ;
	bool operator()(const east::string_t& l, const double& r) const ;
	bool operator()(const east::string_t& l, const east::string_t& r) const ;
	bool operator()(const east::string_t& l, const east::array_v& r) const ;
	bool operator()(const east::string_t& l, const east::map_v& r) const ;

	bool operator()(const east::array_v& l, const bool& r) const ;
	bool operator()(const east::array_v& l, const double& r) const ;
	bool operator()(const east::array_v& l, const east::string_t& r) const ;
	bool operator()(const east::array_v& l, const east::array_v& r) const ;
	bool operator()(const east::array_v& l, const east::map_v& r) const ;

	bool operator()(const east::map_v& l, const bool& r) const ;
	bool operator()(const east::map_v& l, const double& r) const ;
	bool operator()(const east::map_v& l, const east::string_t& r) const ;
	bool operator()(const east::map_v& l, const east::array_v& r) const ;
	bool operator()(const east::map_v& l, const east::map_v& r) const ;
};

} // namespace cppjinja::evtnodes
