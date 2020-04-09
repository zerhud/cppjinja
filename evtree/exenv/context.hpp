/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "evtree/declarations.hpp"
#include "parser/ast/common.hpp"

namespace cppjinja::evt {

class context {
public:
	virtual ~context() noexcept =default ;

	virtual void current_node(const node* n) =0 ;
	virtual const node* current_node(std::size_t ind=0) const =0 ;

	virtual void pop(const node* n)=0 ;
	virtual void push(const node* n)=0 ;
	virtual const node* maker() const =0 ;

	virtual void inject(const evtnodes::callable* n)=0 ;
	virtual void takeout(const evtnodes::callable* n)=0 ;
	virtual std::vector<const evtnodes::callable*> injections() const =0 ;

	virtual std::optional<ast::value_term> solve_var(
	        const cppjinja::ast::var_name& var) const=0 ;
};

class raii_push_ctx final {
	context* ctx;
	const node* maker;
public:
	raii_push_ctx(const node* n, context* c);
	~raii_push_ctx() ;
};

class raii_inject final {
	context* ctx;
	const evtnodes::callable* maker;
public:
	raii_inject(const evtnodes::callable* n, context* c);
	~raii_inject() ;
};

} // namespace cppjinja::evt
