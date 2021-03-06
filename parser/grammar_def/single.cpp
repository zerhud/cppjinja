/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "single.hpp"
#include "opterm.hpp"

#include "../parse.hpp"

namespace cppjinja::text {

BOOST_SPIRIT_INSTANTIATE( decltype(op_out),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(op_comment), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(op_set),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(op_include), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(op_import), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(op_comment_value), iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(op_out),     iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(op_comment), iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(op_set),     iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(op_include), iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(op_import), iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(op_comment_value), iterator_type, context_type_skipper )

} // namespace cppjinja::text

