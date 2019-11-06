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

BOOST_SPIRIT_INSTANTIATE( decltype(block_raw), iterator_type, context_type )

} // namespace cppjinja::text
