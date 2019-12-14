/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "single.hpp"
#include "opterm.hpp"
#include "tmpls.hpp"

#include "../parse.hpp"

namespace cppjinja::text {

BOOST_SPIRIT_INSTANTIATE( decltype(extend_st), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(extend_st_ex), iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(tmpl), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(tmpl_ex), iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(tmpl_original), iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(file), iterator_type, context_type )
} // namespace cppjinja::text

