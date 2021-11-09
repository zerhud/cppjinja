/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <tuple>
#include <type_traits>

#include <boost/callable_traits.hpp>
#include <absd/simple_data_holder.hpp>

#include "../value.hpp"
#include "../builtins.hpp"

namespace cppjinja::evt::context_objects
{

template<typename P, int I>
concept CompatibleParam = absd::AnyData<std::tuple_element_t<I,P>>;

//template<typename P, std::size_t Ind, std::size_t... Inds>
//concept CompatibleParams = CompatibleParam<P, Inds> && ...;

template<typename F>
concept CompatibleFunction =
        std::is_same_v<boost::callable_traits::return_type_t<F>, absd::data>
     || absd::AnyData<boost::callable_traits::return_type_t<F>>
      ;

template<CompatibleFunction Fnc>
class function_adapter {
	Fnc func;
public:
	using params_type = std::pmr::vector<east::function_parameter>;
	using args_t = boost::callable_traits::args_t<Fnc>;

	function_adapter(Fnc&& func) : func(std::forward<Fnc>(func)) {}
	std::shared_ptr<context_object> operator()(params_type params) const
	{
		if(std::tuple_size_v<args_t> != params.size())
			throw std::runtime_error("parameter cound mismatch");
		args_t args = make_args(params, std::make_index_sequence<std::tuple_size_v<args_t>>{});
		auto ret = std::apply(func, args);
		if constexpr (std::is_same_v<decltype(ret), absd::data>)
		    return std::make_shared<value>( std::move(ret) );
		else
		    return std::make_shared<value>( absd::make_simple(std::move(ret)) );
	}
private:
	template<std::size_t... I>
	args_t make_args(const params_type& params, std::index_sequence<I...>) const
	{
		return args_t( ((std::tuple_element_t<I, args_t>)params.at(I).val.value())... );
	}
};

class lambda_function : public builtin_function {
public:
	typedef std::function<
	    std::shared_ptr<context_object>
	    (std::pmr::vector<function_parameter>)>
		fnc_type;
	lambda_function(fnc_type fnc);
	std::shared_ptr<context_object> call(std::pmr::vector<function_parameter> params) const override ;
private:
	fnc_type lambda;
};

} // namespace cppjinja::evt::context_objects
