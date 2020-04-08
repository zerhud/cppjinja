/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <turtle/mock.hpp>

#include "eval/eval.hpp"
#include "evtree/nodes/callable.hpp"

namespace mocks {

MOCK_BASE_CLASS( data_provider, cppjinja::data_provider)
{
	MOCK_METHOD( value, 1, cppjinja::east::value_term(const cppjinja::east::var_name& val), value_var_name )
	MOCK_METHOD( value, 1, cppjinja::east::value_term(const cppjinja::east::function_call& val), value_function_call )
	MOCK_METHOD( filter, 2 )
};

MOCK_BASE_CLASS( node, cppjinja::evt::node )
{
	MOCK_METHOD( rinfo, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( is_leaf, 0 )
	MOCK_METHOD( render, 1 )
};

MOCK_BASE_CLASS( callable_node, cppjinja::evtnodes::callable )
{
	MOCK_METHOD( rinfo, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( is_leaf, 0 )
	MOCK_METHOD( render, 1 )
	MOCK_METHOD( param, 2, std::optional<cppjinja::ast::value_term>(const cppjinja::evt::callstack& ctx,const cppjinja::ast::var_name& name) )
};

} // namespace mocks
