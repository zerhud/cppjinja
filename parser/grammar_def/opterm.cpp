/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "opterm.hpp"
#include "../parse.hpp"

namespace cppjinja::text {

BOOST_SPIRIT_INSTANTIATE( decltype(op_term_start),      iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(op_term_end),        iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_term_start),   iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_term_end),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(comment_term_start), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(comment_term_end),   iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(op_term_start),      iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(op_term_end),        iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(block_term_start),   iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(block_term_end),     iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(comment_term_start), iterator_type, context_type_skipper )
BOOST_SPIRIT_INSTANTIATE( decltype(comment_term_end),   iterator_type, context_type_skipper )

} // namespace cppjinja::text
