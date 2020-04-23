/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../node.hpp"
#include "evtree/exenv/ctx_object.hpp"

namespace cppjinja::evtnodes {

class callable : virtual public evt::node {
protected:
	std::optional<ast::value_term> param(
	          const std::vector<ast::macro_parameter>& params
	        , const ast::var_name& name
	        , const evt::callstack& ctx) const ;
public:
	virtual east::string_t evaluate( evt::exenv& env) const =0 ;
	virtual std::optional<ast::value_term> param(
	          const evt::callstack& ctx
	        , const ast::var_name& name) const =0 ;
};

class callable_solver : public evt::ctx_object {
	evt::exenv* env;
	const callable* block;
public:
	callable_solver(evt::exenv* e, const evtnodes::callable* b);
	~callable_solver() noexcept ;
	ast::value_term call(ast::function_call fnc) override ;
	ast::value_term solve(ast::var_name var) override ;
};

class callable_multisolver : public evt::ctx_object {
	evt::exenv* env;
	std::unordered_map<ast::string_t, const callable*> objs;
public:
	explicit callable_multisolver(evt::exenv* e);
	~callable_multisolver() noexcept ;
	void add(ast::string_t n, const callable* o);
	ast::value_term call(ast::function_call fnc) override ;
	ast::value_term solve(ast::var_name var) override ;
};

} // namespace cppjinja::evtnodes

