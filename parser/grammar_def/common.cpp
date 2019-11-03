/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"
#include "../config.hpp"

namespace cppjinja::text {

BOOST_SPIRIT_INSTANTIATE( decltype(quoted_string), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(single_var_name), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(var_name), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(binary_op1), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(binary_op2), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(value_term_r1), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(value_term_r2), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(function_call), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(function_call_parameter), iterator_type, context_type )

} // namespace cppjinja::text
