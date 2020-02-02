/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/ast/tmpls.hpp"
#include "ast.hpp"

namespace cppjinja {

class data_provider {
public:
	virtual ~data_provider() noexcept =default ;

	virtual east::string_t render(const east::var_name& val) const =0 ;
	virtual east::string_t render(const east::function_call& val) const =0 ;
	virtual east::string_t render(
	          const east::function_call& val
	        , const east::string_t& base
	        ) const =0 ;
};

class evaluator {
public:
	virtual ~evaluator() noexcept =default ;
	virtual evaluator& add_tmpl(ast::tmpl& t) =0 ;
	virtual void render(
	          std::ostream& to
	        , const data_provider& data
	        , const ast::string_t& name
	        ) const =0 ;
};

} // namespace cppjinja