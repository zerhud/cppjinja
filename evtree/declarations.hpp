/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

namespace cppjinja {

/// compiled template
class evtree;

namespace evt {

/// a node of template, compilation unit
class node;

class exenv;

} // namespace evt

namespace evtnodes {

/// a callable node: block, macro..
class callable;

} // namespace evtnodes

} // namespace cppjinja
