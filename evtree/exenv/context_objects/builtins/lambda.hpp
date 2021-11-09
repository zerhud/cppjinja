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

inline std::pair<std::string, std::optional<absd::data>> make_par(std::string n)
{
	std::pair<std::string, std::optional<absd::data>> ret;
	ret.first = std::move(n);
	return ret;
}

template<typename T>
std::pair<std::string, std::optional<absd::data>> make_par(std::string n, T&& v)
{
	std::pair<std::string, std::optional<absd::data>> ret;
	ret.first = std::move(n);
	if constexpr (std::is_same_v<T, absd::data>)
	        ret.second = std::move(v);
	else ret.second = absd::make_simple(std::forward<T>(v));
	return ret;
}

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
	using values_t = std::array<std::pair<std::string,std::optional<absd::data>>, std::tuple_size_v<args_t>>;
	template<std::size_t I>
	using param_t = std::tuple_element_t<I, args_t>;
private:
	Fnc func;
	std::optional<values_t> names;
public:

	function_adapter(Fnc&& func) : func(std::forward<Fnc>(func)) {}
	function_adapter(Fnc&& func, values_t name_list)
	    : func(std::forward<Fnc>(func))
	    , names(name_list)
	{}

	constexpr std::size_t arity() const
	{
		return std::tuple_size_v<args_t>;
	}

	std::shared_ptr<context_object> operator()(params_type params) const
	{
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

	template<std::size_t... I>
	args_t make_args(params_type& params, std::size_t npc, std::index_sequence<I...>) const
	{
		return args_t( extract_param<I>(params, npc)... );
	}

	template<std::size_t I>
	param_t<I> extract_param(params_type& params, std::size_t named_params_count) const
	{
		auto named = try_extract_named_param<I>(params);
		if(named) return *named;
		auto def = try_extract_def_param<I>(params);
		if(def) return *def;
		return (param_t<I>)params.at(I - named_params_count).val.value();
	}

	template<std::size_t I>
	std::optional<param_t<I>> try_extract_named_param(params_type& params) const
	{
		if(names.has_value()) {
			for(auto pos = params.begin();pos!=params.end();++pos) {
				if(pos->name && *pos->name == (*names)[I].first) {
					return (param_t<I>)pos->val.value();
				}
			}
		}
		return std::nullopt;
	}
	template<std::size_t I>
	std::optional<param_t<I>> try_extract_def_param(params_type& params) const
	{
		if(params.size() <= I) {
			if(names && (*names)[I].second) {
				return (param_t<I>)(*names)[I].second.value();
			}
			throw std::runtime_error("parameter count mismatch");
		}
		return std::nullopt;
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
