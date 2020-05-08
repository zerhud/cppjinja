/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../expr_eval.hpp"

namespace cppjinja::evt::expr_evals {

template<typename T>
class expr_eval_to_any {
public:
	typedef std::optional<T> ret_t;
	typedef T result_t;

	template<typename Val>
	std::enable_if_t<!std::is_same_v<std::decay_t<Val>,T>, std::optional<T>>
	operator()(Val&&)
	{
		return std::nullopt;
	}

	template<typename Val>
	std::enable_if_t<std::is_same_v<std::decay_t<Val>,T>, std::optional<T>>
	operator()(Val&& v)
	{
		return v;
	}
};

} // namespace cppjinja::evt::expr_evals
