/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "eval.hpp"
#include "parser/helpers.hpp"

using namespace std::literals;

namespace cppjinja::details {

struct name_check_visitor : boost::static_visitor<bool> {
	ast::string_t name;

	template<typename O>
	std::enable_if_t<has_str_name_v<O>, bool>
	    operator()(const O& o) {return o.name==name;}

	template<typename O>
	std::enable_if_t<!has_str_name_v<O>, bool>
	    operator()(const O&) {return false;}
};

} // namespace cppjinja::details

const cppjinja::ast::block_content* cppjinja::evaluator::search_by_name(const ast::var_name& name) const
{
	assert(!data_stack_.empty());
	assert(!name.empty());

	details::name_check_visitor check_last_name;
	check_last_name.name = name.back();

	for(auto cur=data_stack_.rbegin();cur!=data_stack_.rend();++cur)
	{
		assert(*cur);
		const std::vector<ast::block_content>& cur_cnt = *(*cur);
		for(const auto& cnt:cur_cnt)
		{
			const bool found = boost::apply_visitor(check_last_name, cnt);
			if(found) return &cnt;
		}
	}

	return nullptr;
}

std::string cppjinja::evaluator::render(const cppjinja::ast::block_content& cnt, bool render_data) const
{
//	, forward_ast<block_raw>
//	, forward_ast<block_if>
//	, forward_ast<block_for>
//	, forward_ast<block_macro>
//	, forward_ast<block_named>
//	, forward_ast<block_filtered>
//	, forward_ast<block_set>
//	, forward_ast<block_call>

	overloaded cnt_visitor {
		  [](const ast::string_t& s){ return s;}
		, [this](const ast::op_out& o){ return render(o.value, o.filters); }
		, [](const ast::op_comment&) { return ""s; }
		, [&render_data,this](const ast::op_set& o){ return render_data ? render(o.value, {}) : ""s;}
		, [](const auto&){ return ""; }
	};

	return boost::apply_visitor(cnt_visitor, cnt.var);
}

void cppjinja::evaluator::render(std::ostream& to, const std::vector<cppjinja::ast::block_content>& cnt) const
{

	data_stack_.push_back(&cnt);
	for(auto& cnt:cnt) to << render(cnt, false);
	data_stack_.pop_back();
}

std::string cppjinja::evaluator::render(const cppjinja::ast::value_term& val, const std::vector<ast::filter_call>& filters) const
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

	return base;
}

std::string cppjinja::evaluator::render(const cppjinja::ast::var_name& var) const
{
	assert(data_);
	const ast::block_content* found_var = search_by_name(var);
	return found_var ? render(*found_var, true) : data_->solve(var);
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

	data_ = &data;
	render(to, tmpl_->content);
	data_ = nullptr;
}
