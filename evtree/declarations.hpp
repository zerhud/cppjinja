/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <json.hpp>

namespace cppjinja {

using json = nlohmann::json;

/// compiled template
class evtree;

namespace evt {

/// a node of template, compilation unit
class node;

/// environment for execution
class exenv;
class context;
class callstack;
class ctx_object;
class context_object; ///< aka namespace

class result_formatter;

class factory {
public:
	virtual ~factory() noexcept =default ;
	virtual std::unique_ptr<exenv> craete_execution_env() const =0 ;
};

} // namespace evt

namespace evtnodes {

class tmpl;
/// a callable node: block, macro..
class callable;
class callable_multisolver;

} // namespace evtnodes

} // namespace cppjinja
