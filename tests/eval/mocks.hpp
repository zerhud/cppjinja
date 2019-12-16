/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <turtle/mock.hpp>

#include "evaluator/eval.hpp"

namespace mocks {

MOCK_BASE_CLASS( data_provider, cppjinja::data_provider)
{
    MOCK_METHOD( solve, 1, std::string(const cppjinja::ast::var_name&), solve_var_name )
    MOCK_METHOD( solve, 1, std::string(const cppjinja::ast::function_call&), solve_function_call )
    MOCK_METHOD( solve, 2, std::string(const cppjinja::ast::function_call&, const std::string&), solve_filter_call )
};

} // namespace mocks
