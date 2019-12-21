/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block.hpp"
#include "parser/helpers.hpp"

#include "evaluator/eval.hpp"
#include "../ast_cvt.hpp"

using namespace cppjinja::eval_tree;
using namespace std::literals;

block::block(all_blocks cur, const evaluate_tree* et)
    : cur_block_(std::move(cur))
    , evaluator_(et)
{
	assert(et);
}

std::optional<cppjinja::ast::string_t> block::name() const
{
	const ast::block_named* named = std::get_if<ast::block_named>(&cur_block_);
	if(named) return named->name;
	return std::nullopt;
}

cppjinja::ast::string_t block::render(const std::vector<cppjinja::ast::macro_parameter>& params, const data_provider& datas) const
{
//	, forward_ast<block_if>
//	, forward_ast<block_for>
//	, forward_ast<block_macro>
//	, forward_ast<block_filtered>
//	, forward_ast<block_set>
//	, forward_ast<block_call>

	data_ = &datas;

	overloaded cnt_visitor {
		  [](const ast::string_t& s){ return s;}
		, [this](const ast::op_out& o){ return render(o.value, o.filters); }
		, [](const ast::op_comment&) { return ""s; }
		, [](const ast::op_set&){ return ""s;}
		, [](const ast::forward_ast<ast::block_raw>& o){ return o.get().value; }
		//, [this](const ast::forward_ast<ast::block_named>& o){ return render(o.get()); }
		, [](const auto&){ return "default_block"; }
	};

	ast::string_t ret;

	overloaded block_visitor {
		  [&ret](const ast::block_raw& b){ ret = b.value; }
		, [&ret,&cnt_visitor](const auto& bl){ for(auto&c:bl.content) ret+=boost::apply_visitor(cnt_visitor, c.var); }
	};


	std::visit(block_visitor, cur_block_);

	data_ = nullptr;
	return ret;
}

cppjinja::ast::string_t block::variable(cppjinja::ast::string_t name) const
{
	auto iterate_content = [&name,this](auto& obj)
	{
		if constexpr (has_blockcontent_content_v<decltype(obj)>) {
			for(auto& c:obj.content) {
				const ast::op_set* set = boost::get<ast::op_set>(&c);
				if(set && set->name==name) return render(set->value, {});
			}
		}
		assert(false);
		return ""s;
	};

	return std::visit(iterate_content, cur_block_);
}

bool block::has_variable(cppjinja::ast::string_t name) const
{
	auto iterate_content = [&name](auto& obj)
	{
		if constexpr (has_blockcontent_content_v<decltype(obj)>) {
			for(auto& c:obj.content) {
				const ast::op_set* set = boost::get<ast::op_set>(&c);
				if(set && set->name==name) return true;
			}
		}
		return false;
	};

	return std::visit(iterate_content, cur_block_);
}

cppjinja::ast::string_t block::render(const cppjinja::ast::value_term& val, const std::vector<cppjinja::ast::filter_call>& filters) const
{
	overloaded visitor {
		  [](const ast::string_t& v){ return v;}
		, [](const double& v){ return std::to_string(v);}
		, [](const ast::tuple_v&){ return ""; }
		, [](const ast::array_v&){ return ""; }
		, [this](const ast::var_name& v){ return render(v); }
		, [this](const ast::function_call& v){ return render(v); }
		, [this](const ast::binary_op& v){ return render(v); }
	};

	ast::string_t base = boost::apply_visitor(visitor, val.var);
	for(auto& f:filters) base = render(base, f);

	return base;
}

cppjinja::ast::string_t block::render(const cppjinja::ast::var_name& var) const
{
	assert(data_);
	if(var.size()==1 && has_variable(var[0]))
		return variable(var[0]);
	return data_->render(var);
}

cppjinja::ast::string_t block::render(const cppjinja::ast::function_call& var) const
{
	assert(data_);
	//const ast::block_content* cnt = search_by_name(var.ref);

	//if(!cnt) return data_->render(details::east_cvt::cvt(var));

	//const bool is_macro = cnt->var.type() == typeid(ast::block_macro);
	//const bool is_call = cnt->var.type() == typeid(ast::block_call);
	//if(!is_macro && !is_call) throw std::runtime_error("function_call refers to a not callable object");

	return "render_fnc_call";
}

cppjinja::ast::string_t block::render(const cppjinja::ast::binary_op& var) const
{
	return "render_binary_op";
}

cppjinja::ast::string_t block::render(const std::string& base, const cppjinja::ast::filter_call& filter) const
{
	ast::function_call call;

	if(filter.var.type() == typeid(ast::var_name))
		call.ref = boost::get<ast::var_name>(filter.var);
	else call = boost::get<ast::function_call>(filter.var);

	return data_->render(details::east_cvt::cvt(call), base);
}
