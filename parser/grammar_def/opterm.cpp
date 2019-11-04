/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "opterm.hpp"
#include "../config.hpp"

namespace cppjinja::text {

BOOST_SPIRIT_INSTANTIATE( decltype(op_term_start), iterator_type, context_type )

} // namespace cppjinja::text
