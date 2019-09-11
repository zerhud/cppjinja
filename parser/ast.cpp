/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast.hpp"

using namespace std::literals;

cppjinja::macro_parameter::macro_parameter(std::string n)
	: name(std::move(n))
	, value_t(type::no)
{
}

cppjinja::macro_parameter::macro_parameter(std::string n, std::string v)
	: name(std::move(n))
	, value(std::move(v))
	, value_t(type::str)
{
}

