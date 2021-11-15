/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
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

#include "../builtins.hpp"

namespace cppjinja::evt::context_objects
{

template<typename P, int I>
constexpr bool CompatibleParam = // must copy value
    (   !std::is_reference_v<std::tuple_element_t<I,P>>
     && !std::is_pointer_v<std::tuple_element_t<I,P>>
    ) && // absd::data or convertible to absd::data
    (   absd::AnyData<std::tuple_element_t<I,P>>
     || std::is_same_v<std::tuple_element_t<I,P>, absd::data>
    );

template<typename P, std::size_t... Inds>
constexpr bool CompatibleParams(std::index_sequence<Inds...>) {return false;}
template<typename P, std::size_t... Inds> requires requires(std::index_sequence<Inds...>){
	requires (CompatibleParam<P, Inds> && ...);
}
constexpr bool CompatibleParams(std::index_sequence<Inds...>) {return true;}

template<typename F>
concept CompatibleFunction =
        (
            std::is_same_v<boost::callable_traits::return_type_t<F>, absd::data>
         || absd::AnyData<boost::callable_traits::return_type_t<F>>
        )
     && (
            std::tuple_size_v<boost::callable_traits::args_t<F>> == 0
         || CompatibleParams<boost::callable_traits::args_t<F>>(std::make_index_sequence<std::tuple_size_v<boost::callable_traits::args_t<F>>>())
        )
      ;

inline std::pair<std::pmr::string, std::optional<absd::data>> make_par(std::pmr::string n)
{
	std::pair<std::pmr::string, std::optional<absd::data>> ret;
	ret.first = std::move(n);
	return ret;
}

template<typename T> requires (CompatibleParam<std::tuple<T>, 0>)
std::pair<std::pmr::string, std::optional<absd::data>> make_par(std::pmr::string n, T&& v)
{
	std::pair<std::pmr::string, std::optional<absd::data>> ret;
	ret.first = std::move(n);
	if constexpr (std::is_same_v<T, absd::data>)
		ret.second = std::move(v);
	else ret.second = absd::make_simple(std::forward<T>(v));
	return ret;
}

template<CompatibleFunction Fnc>
class function_adapter {
public:
	using params_type = std::pmr::vector<east::function_parameter>;
	using args_t = boost::callable_traits::args_t<Fnc>;
	using values_t = std::array<std::pair<std::pmr::string,std::optional<absd::data>>, std::tuple_size_v<args_t>>;
	template<std::size_t I>
	using param_t = std::tuple_element_t<I, args_t>;
private:
	Fnc func;
	std::optional<values_t> names;
public:
	function_adapter(const function_adapter&) =default ;
	function_adapter& operator = (const function_adapter&) =default ;

	function_adapter(function_adapter&&) noexcept =default ;
	function_adapter& operator = (function_adapter&&) noexcept =default ;

	function_adapter(Fnc&& func) : func(std::forward<Fnc>(func)) {}
	function_adapter(Fnc&& func, values_t name_list)
	    : func(std::forward<Fnc>(func))
	    , names(name_list)
	{
	}

	template<typename... Params>
	function_adapter(Fnc&& fnc, Params&&... p)
	    : function_adapter(std::forward<Fnc>(fnc), {std::forward<Params>(p)...})
	{
	}

	static constexpr std::size_t arity()
	{
		return std::tuple_size_v<args_t>;
	}

	absd::data operator()(params_type params) const
	{
		sort_params(params);
		args_t args = make_args(params, std::make_index_sequence<arity()>{});
		auto ret = std::apply(func, args);
		if constexpr (std::is_same_v<decltype(ret), absd::data>)
		    return ret;
		else
		    return absd::make_simple(std::move(ret));
	}
private:
	void sort_params(params_type& params) const
	{
		if(!names) {
			if(arity() != params.size())
				throw std::runtime_error("parameter count mismatch");
			return;
		}

		params_type sorted;
		std::size_t del_count = 0;
		for(std::size_t i=0;i<names->size();++i) {
			std::size_t ind = i - del_count;
			auto named = try_extract_named_param(i, params);
			if(named) {
				sorted.emplace_back().val = std::move(*named);
				++del_count;
			}
			else if(params.size() <= ind) {
				if((*names)[i].second)
					sorted.emplace_back().val = (*names)[i].second;
				else throw std::runtime_error("parameter count mismatch");
			}
			else sorted.emplace_back().val = std::move(params[ind].val.value());
		}
		params = std::move(sorted);
	}

	template<std::size_t... I>
	args_t make_args(params_type& params, std::index_sequence<I...>) const
	{
		assert(sizeof...(I) == params.size());
		return args_t( extract_param<I>(params)... );
	}

	template<std::size_t I>
	param_t<I> extract_param(params_type& params) const
	{
		assert( I < params.size() );
		return (param_t<I>)params[I].val.value();
	}

	std::optional<absd::data> try_extract_named_param(
	        std::size_t i, params_type& params) const
	{
		assert(names.has_value());
		if(names.has_value()) {
			for(auto pos = params.begin();pos!=params.end();++pos) {
				if(pos->name && *pos->name == (*names)[i].first) {
					auto ret = pos->val.value();
					params.erase(pos);
					return ret;
				}
			}
		}
		return std::nullopt;
	}
};

} // namespace cppjinja::evt::context_objects
