/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "node.hpp"

using namespace std::literals;

void cppjinja::node::add_parent(cppjinja::node* np)
{
	assert(
	      std::find(
	                std::begin(parents_),
	                std::end(parents_),
	                np) == std::end(parents_)
	      );

	parents_.emplace_back(np);
}

std::vector<cppjinja::node*> cppjinja::node::parents()
{
	return parents_;
}

std::vector<const cppjinja::node*> cppjinja::node::parents() const
{
	std::vector<const cppjinja::node*> ret;
	for(auto& p:parents_) ret.emplace_back(p);
	return ret;
}

bool cppjinja::node::is_parent(const cppjinja::node* n) const
{
	for(auto& p:parents_) if(p==n) return true;
	return false;
}
