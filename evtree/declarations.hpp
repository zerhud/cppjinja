/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>

namespace cppjinja {

/// compiled template
class evtree;

namespace evt {

/// a node of template, compilation unit
class node;

/// environment for execution
class exenv;
class context;
class callstack;

class factory {
public:
	virtual ~factory() noexcept =default ;
	virtual std::unique_ptr<exenv> craete_execution_env() const =0 ;
};

} // namespace evt

namespace evtnodes {

/// a callable node: block, macro..
class callable;

} // namespace evtnodes

} // namespace cppjinja