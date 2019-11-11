/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "blocks.hpp"
#include "../parse.hpp"

namespace cppjinja::text {

BOOST_SPIRIT_INSTANTIATE( decltype(block_if),          iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_for),         iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_raw),         iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_macro),       iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_named),       iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_content),     iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_filtered),    iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(macro_parameter),   iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(block_content_vec), iterator_type, context_type )

} // namespace cppjinja::text

