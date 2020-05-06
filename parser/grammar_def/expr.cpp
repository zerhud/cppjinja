/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr.hpp"
#include "../parse.hpp"

namespace cppjinja::text::expr_ops {

BOOST_SPIRIT_INSTANTIATE( decltype(bool_rule), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(term),      iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(quoted_string), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(math),      iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(mathop),    iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(single_var_name), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(point),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(concat),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(in_check),     iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(list),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(tuple),    iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(dict),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(eq_assign),iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(in_assign),iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(expr), iterator_type, context_type )

} // namespace cppjinja::text::expr_ops
