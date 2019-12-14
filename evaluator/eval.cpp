/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "eval.hpp"
#include "parser/helpers.hpp"

const cppjinja::ast::block_content* cppjinja::evaluator::search_by_name(const ast::var_name& name) const
{
	return nullptr;
}

void cppjinja::evaluator::render(std::ostream& to, const cppjinja::ast::value_term& val, const std::vector<ast::filter_call>& filters) const
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

	std::string base = boost::apply_visitor(visitor, val.var);
	for(auto& f:filters) base = render(base, f);

	to << base;
}

std::string cppjinja::evaluator::render(const cppjinja::ast::var_name& var) const
{
	assert(data_);
	return data_->solve(var);
}

std::string cppjinja::evaluator::render(const cppjinja::ast::function_call& var) const
{
	assert(data_);
	const ast::block_content* cnt = search_by_name(var.ref);
	if(!cnt) return data_->solve(var);
	const bool is_macro = cnt->var.type() == typeid(ast::block_macro);
	const bool is_call = cnt->var.type() == typeid(ast::block_call);
	if(!is_macro && !is_call) throw std::runtime_error("function_call refers to a not callable object");

	return "function call";
}

std::string cppjinja::evaluator::render(const cppjinja::ast::binary_op& var) const
{
	return "binary_op";
}

std::string cppjinja::evaluator::render(const std::string& base, const cppjinja::ast::filter_call& filter) const
{
	return "filter call";
}

cppjinja::evaluator::evaluator(std::vector<cppjinja::ast::tmpl> tmpls)
    : tmpls_(std::move(tmpls))
{
	if(tmpls_.empty()) throw std::runtime_error("must to be a template to evaluate");
	tmpl_ = &tmpls_[0];
}

void cppjinja::evaluator::render(std::ostream& to, const cppjinja::data_provider& data) const
{
	assert(tmpl_);
//	, op_comment
//	, op_set
//	, forward_ast<block_raw>
//	, forward_ast<block_if>
//	, forward_ast<block_for>
//	, forward_ast<block_macro>
//	, forward_ast<block_named>
//	, forward_ast<block_filtered>
//	, forward_ast<block_set>
//	, forward_ast<block_call>

	data_ = &data;

	overloaded cnt_visitor {
		  [&to](const ast::string_t& s){ to << s;}
		, [&to,this](const ast::op_out& o){ render(to, o.value, o.filters); }
		, [&to](const auto&){ to << ""; }
	};

	for(auto& cnt:tmpl_->content) boost::apply_visitor(cnt_visitor, cnt.var);

	data_ = nullptr;
}
