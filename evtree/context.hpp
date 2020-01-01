/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/eval.hpp"

namespace cppjinja {

class node;

namespace evt {

class context {
	std::vector<std::vector<const node*>> ctx;
	std::vector<const node*> callstack;
	const data_provider* prov;
public:
	context(const data_provider* p);

	const data_provider& data() const ;

	void pop_context();
	void push_context();
	void add_context(const node* n);
	std::vector<const node*> ctx_all() const;
	const node* ctx_last() const ;

	void pop_callstack(const node* n);
	void push_callstack(const node* n);
};

}} // namespace cppjinja::evtree
