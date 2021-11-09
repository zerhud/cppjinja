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
public:
	using params_type = std::pmr::vector<east::function_parameter>;
	using args_t = boost::callable_traits::args_t<Fnc>;
	using names_t = std::array<std::string, std::tuple_size_v<args_t>>;
	template<std::size_t I>
	using param_t = std::tuple_element_t<I, args_t>;
private:
	Fnc func;
	std::optional<names_t> names;
public:

	function_adapter(Fnc&& func) : func(std::forward<Fnc>(func)) {}
	function_adapter(Fnc&& func, names_t name_list)
	    : func(std::forward<Fnc>(func))
	    , names(name_list)
	{}

	constexpr std::size_t arity() const
	{
		return std::tuple_size_v<args_t>;
	}

	std::shared_ptr<context_object> operator()(params_type params) const
	{
		if(std::tuple_size_v<args_t> != params.size())
			throw std::runtime_error("parameter cound mismatch");
		std::size_t named_params_count = sort_params(params);
		args_t args = make_args(
		            params, named_params_count,
		            std::make_index_sequence<std::tuple_size_v<args_t>>{});
		auto ret = std::apply(func, args);
		if constexpr (std::is_same_v<decltype(ret), absd::data>)
		    return std::make_shared<value>( std::move(ret) );
		else
		    return std::make_shared<value>( absd::make_simple(std::move(ret)) );
	}
private:
	template<std::size_t... I>
	args_t make_args(params_type& params, std::size_t npc, std::index_sequence<I...>) const
	{
		return args_t( extract_param<I>(params, npc)... );
	}

	template<std::size_t I>
	param_t<I> extract_param(params_type& params, std::size_t named_params_count) const
	{
		if(names.has_value()) {
			for(auto pos = params.begin();pos!=params.end();++pos) {
				if(pos->name && *pos->name == (*names)[I]) {
					return (param_t<I>)pos->val.value();
				}
			}
		}
		return (param_t<I>)params.at(I-named_params_count).val.value();
	}

	std::size_t sort_params(params_type& params) const
	{
		std::stable_sort(params.begin(), params.end(),
		                 [](auto& a, auto& b){
			return !a.name.has_value() && b.name.has_value();
		});
		std::size_t ret = 0;
		for(auto& p:params) if(p.name.has_value()) ++ret;
		return ret;
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
