/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "expr.hpp"
#include <iomanip>

std::ostream& cppjinja::ast::expr_ops::operator <<(std::ostream& out, cppjinja::ast::expr_ops::math_op op)
{
	return out << static_cast<int>(op);
}
std::ostream& cppjinja::ast::expr_ops::operator <<(std::ostream& out, cppjinja::ast::expr_ops::logic_op op)
{
	return out << static_cast<int>(op);
}
std::ostream& cppjinja::ast::expr_ops::operator <<(std::ostream& out, cppjinja::ast::expr_ops::cmp_op op)
{
	return out << static_cast<int>(op);
}

std::ostream& cppjinja::ast::expr_ops::operator <<(std::ostream& out, const cppjinja::ast::expr_ops::expr& op)
{
	out << "expr " << op.var.type().name() << ": ";
	auto prt = [&out](const auto& v)->std::ostream&{return out << v;};
	return boost::apply_visitor(prt, op.var);
}

bool cppjinja::ast::expr_ops::operator ==(const cppjinja::ast::expr_ops::expr& left, const cppjinja::ast::expr_ops::expr& right)
{
	auto cmp = [](const auto& l, const auto& r)
	{
		if constexpr(std::is_same_v<decltype(l), decltype(r)>) return l == r;
		else return false;
	};
	return boost::apply_visitor(cmp, left.var, right.var);
}

bool cppjinja::ast::expr_ops::operator ==(const cppjinja::ast::expr_ops::term& left, const cppjinja::ast::expr_ops::term& right)
{
	return left.var == right.var;
}

bool cppjinja::ast::expr_ops::operator ==(const cppjinja::ast::expr_ops::list& left, const cppjinja::ast::expr_ops::list& right)
{
	return left.items == right.items;
}

bool cppjinja::ast::expr_ops::operator == (const tuple& left, const tuple& right)
{
	return left.items == right.items;
}

bool cppjinja::ast::expr_ops::operator == (const dict& left, const dict& right)
{
	if(left.items.size() != right.items.size()) return false;
	for(std::size_t i=0;i<left.items.size();++i) {
		if(left.items[i].name != right.items[i].name) return false;
		if(!(left.items[i].value.get() == right.items[i].value.get())) return false;
	}
	return true;
}

bool cppjinja::ast::expr_ops::operator == (const lvalue& left, const lvalue& right){ return left.var == right.var; }
bool cppjinja::ast::expr_ops::operator == (const any_assign& left, const any_assign& right)
{
	return left.names == right.names && left.value == right.value;
}
bool cppjinja::ast::expr_ops::operator == (const math& left, const math& right)
{
	return left.op == right.op && left.left == right.left && left.right == right.right;
}
bool cppjinja::ast::expr_ops::operator == (const concat& left, const concat& right)
{
	return left.left == right.left && left.right == right.right;
}
bool cppjinja::ast::expr_ops::operator == (const in_check& left, const in_check& right)
{
	return left.term.get() == right.term.get() && left.object.get() == right.object.get();
}
bool cppjinja::ast::expr_ops::operator == (const cmp_check& left, const cmp_check& right)
{
	return left.op == right.op && left.left == right.left && left.right == right.right;
}
bool cppjinja::ast::expr_ops::operator == (const logic_check& left, const logic_check& right)
{
	return left.op == right.op && left.left == right.left && left.right == right.right;
}
bool cppjinja::ast::expr_ops::operator == (const negate& left, const negate& right){return left.arg == right.arg;}
bool cppjinja::ast::expr_ops::operator == (const fnc_call& left, const fnc_call& right)
{
	return left.ref == right.ref && left.args == right.args;
}
bool cppjinja::ast::expr_ops::operator == (const filter_call& left, const filter_call& right)
{
	return left.ref == right.ref && left.args == right.args;
}
bool cppjinja::ast::expr_ops::operator == (const filter& left, const filter& right){return left.base == right.base && left.filters == right.filters;}
bool cppjinja::ast::expr_ops::operator == (const single_var_name& left, const single_var_name& right){return left.name == right.name;}
bool cppjinja::ast::expr_ops::operator == (const point_element& left, const point_element& right)
{
	auto cmp = [](const auto& l, const auto& r) {
		if constexpr (std::is_same_v<decltype(l),decltype(r)>) return l == r;
		else return false;
	};
	return boost::apply_visitor(cmp, left, right);
}
bool cppjinja::ast::expr_ops::operator == (const point& left, const point& right)
{
	return left.left == right.left && left.right == right.right;
}
bool cppjinja::ast::expr_ops::operator == (const op_if& left, const op_if& right)
{
	return left.cond == right.cond && left.term == right.term && left.alternative == right.alternative;
}

std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const term& right)
{
	auto prt = [&out](const auto& v) -> std::ostream& {
		if constexpr(std::is_same_v<decltype(v), double>) return out << std::to_string(v);
		else return out << v;
	};
	return boost::apply_visitor(prt, right.var);
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const list& right)
{
	out << '[';
	for(auto& i:right.items) out << i.get() << ',';
	return out << ']';
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const tuple& right)
{
	out << '(';
	for(auto& i:right.items) out << i << ',';
	return out << ')';
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const dict& right)
{
	out << '{';
	for(auto& i:right.items) out << std::quoted(i.name) << ":" << i.value << ',';
	return out << '}';
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const lvalue& right){return out << right.var;}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const expr_bool& op)
{
	out << "expr_bool " << op.var.type().name() << ": ";
	auto prt = [&out](const auto& v)->std::ostream&{return out << v;};
	return boost::apply_visitor(prt, op.var);
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const any_assign& right)
{
	for(auto&n:right.names) out << n << ',';
	return out << '=' << right.value;
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const math& right){return out << right.left << ' ' << right.op << ' ' << right.right;}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const concat& right){return out << right.left << '~' << right.right;}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const in_check& right){return out << right.term.get() << " in " << right.object.get();}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const cmp_check& right)
{
	return out << right.left << " cmp" << static_cast<int>(right.op) << right.right;
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const logic_check& right)
{
	return out << "logic: " << right.left << ' ' << static_cast<int>(right.op) << ' ' << right.right;
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const negate& right){return out << '!' << right.arg;}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const fnc_call& right){return out << right.ref << "(...)";}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const filter_call& right){return out << right.ref << "(...)";}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const filter& right){return out << right.base << '|';}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const point& right){return out << right.left << '.' << right.right;}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const point_element& right){
	return boost::apply_visitor([&out](const auto& v)->std::ostream&{ return out << v; }, right);
}
std::ostream& cppjinja::ast::expr_ops::operator << (std::ostream& out, const op_if& right)
{
	out << right.term << " if " << right.cond;
	if(right.alternative) out << " else " << *right.alternative;
	return out;
}

bool cppjinja::ast::expr_ops::operator ==(const cppjinja::ast::expr_ops::expr_bool& left, const cppjinja::ast::expr_ops::expr_bool& right)
{
	auto cmp = [](const auto& l, const auto& r)
	{
		if constexpr(std::is_same_v<decltype(l), decltype(r)>) return l == r;
		else return false;
	};
	return boost::apply_visitor(cmp, left.var, right.var);
}
