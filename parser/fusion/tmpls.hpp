/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/tmpls.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT( cppjinja::ast::extend_st, tmpl_name, file_name)
BOOST_FUSION_ADAPT_STRUCT( cppjinja::ast::tmpl, name, extends, content)
BOOST_FUSION_ADAPT_STRUCT( cppjinja::ast::file, includes, imports, tmpls)
