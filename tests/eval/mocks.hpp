/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <turtle/mock.hpp>

#include "eval/eval.hpp"

namespace mocks {

MOCK_BASE_CLASS( data_provider, cppjinja::data_provider)
{
	MOCK_METHOD( value, 1, cppjinja::east::value_term(const cppjinja::east::var_name& val), value_var_name )
	MOCK_METHOD( value, 1, cppjinja::east::value_term(const cppjinja::east::function_call& val), value_function_call )
	MOCK_METHOD( filter, 2 )
	MOCK_METHOD( render, 1, std::string(const cppjinja::east::var_name&), render_var_name )
	MOCK_METHOD( render, 2, std::string(const cppjinja::east::function_call&, const cppjinja::east::string_t&), render_filter_call )
};

} // namespace mocks
