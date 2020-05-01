/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "node_tree.hpp"

#include <exception>

void cppjinja::evt::node_tree_details::throw_root_exists()
{
	throw std::runtime_error("the root already exists");
}

void cppjinja::evt::node_tree_details::throw_root_not_exists()
{
	throw std::runtime_error("no such parent root");
}
