/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/ast/tmpls.hpp"

namespace cppjinja {

class node {
	std::vector<cppjinja::node*> parents_;
public:
	virtual ~node() noexcept =default ;

	virtual ast::string_t name() const =0 ;
	virtual bool is_leaf() const =0 ;

	void add_parent(node* np) ;
	std::vector<node*> parents() ;
	std::vector<const node*> parents() const ;
	bool is_parent(const node* n) const ;
};

} // namespace cppjinja
