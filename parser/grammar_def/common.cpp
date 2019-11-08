/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"
#include "opterm.hpp"
#include "../parse.hpp"

namespace cppjinja::text {

decltype(x3::char_)& char_ = x3::char_;
decltype(x3::space)& space = x3::space;

BOOST_SPIRIT_INSTANTIATE( decltype(quoted_string), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(single_var_name), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(var_name), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(binary_op), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(value_term), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(function_call), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(function_call_parameter), iterator_type, context_type )

} // namespace cppjinja::text
