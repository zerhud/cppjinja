/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "binary_op.hpp"

using namespace cppjinja::evtnodes;
using cppjinja::ast::comparator;

void binary_op_helper::throw_compare_to_nothing() const
{
	throw std::runtime_error("cannot compare bool and map");
}

binary_op_helper::binary_op_helper(ast::comparator op)
    : cmp_(op)
{
}

bool binary_op_helper::operator()
(const cppjinja::east::nothing_t&, const cppjinja::east::nothing_t&) const
{
	throw_compare_to_nothing();
}

bool binary_op_helper::operator()(const bool& v) const
{
	return v;
}
bool binary_op_helper::operator()(const double& v) const
{
	return v == .0;
}
bool binary_op_helper::operator()(const east::string_t& v) const
{
	return !v.empty();
}
bool binary_op_helper::operator()(const east::array_v& v) const
{
	return !v.items.empty();
}
bool binary_op_helper::operator()(const east::map_v& v) const
{
	return !v.items.empty();
}

bool binary_op_helper::operator()(const cppjinja::east::nothing_t&) const
{
	throw std::runtime_error("cannot compare with nothin");
}

bool binary_op_helper::operator()(const bool& l, const bool& r) const
{
	return default_cmp(l,r);
}
bool binary_op_helper::operator()(const bool& l, const double& r) const
{
	return default_cmp(l,r);
}
bool binary_op_helper::operator()(const bool& l, const east::string_t& r) const
{
	return default_cmp(l,std::stod(r));
}
bool binary_op_helper::operator()(const bool& l, const east::array_v& r) const
{
	if(cmp_==comparator::in) return default_in(l, r);
	throw std::runtime_error("cannot compare bool and array");
}
bool binary_op_helper::operator()(const bool&, const east::map_v&) const
{
	throw_compare_to_nothing();
}

bool binary_op_helper::operator()(const double& l, const bool& r) const
{
	return default_cmp(l,r);
}
bool binary_op_helper::operator()(const double& l, const double& r) const
{
	return default_cmp(l,r);
}
bool binary_op_helper::operator()(const double& l, const east::string_t& r) const
{
	return default_cmp(l,std::stod(r));
}
bool binary_op_helper::operator()(const double& l, const east::array_v& r) const
{
	if(cmp_ == comparator::in) return default_in(l, r);
	throw std::runtime_error("cannot compare double and array");
}
bool binary_op_helper::operator()(const double& l, const east::map_v& r) const
{
	if(cmp_ == comparator::in)
	{
		for(const auto& [k,v]:r.items) {
			if(!std::holds_alternative<double>(k)) continue;
			if(std::get<double>(k)==l) return true;
		}
		return false;
	}

	throw std::runtime_error("cannot compare double and map");
}

bool binary_op_helper::operator()(const east::string_t& l, const bool& r) const
{
	return default_cmp(!l.empty(), r);
}
bool binary_op_helper::operator()(const east::string_t& l, const double& r) const
{
	return default_cmp(std::stod(l), r);
}
bool binary_op_helper::operator()(const east::string_t& l, const east::string_t& r) const
{
	return default_cmp(l,r);
}
bool binary_op_helper::operator()(const east::string_t& l, const east::array_v& r) const
{
	if(cmp_ == comparator::in) return default_in(l, r);
	throw std::runtime_error("cannot compare string and array");
}
bool binary_op_helper::operator()(const east::string_t& l, const east::map_v& r) const
{
	if(cmp_ == comparator::in)
	{
		for(const auto& [k,v]:r.items) {
			if(!std::holds_alternative<east::string_t>(k)) continue;
			if(std::get<east::string_t>(k)==l) return true;
		}
		return false;
	}
	throw std::runtime_error("cannot compare string and array");
}

bool binary_op_helper::operator()(const east::array_v&, const bool&) const
{
	throw std::runtime_error("cannot compare array and bool");
}
bool binary_op_helper::operator()(const east::array_v&, const double&) const
{
	throw std::runtime_error("cannot compare array and double");
}
bool binary_op_helper::operator()(const east::array_v&, const east::string_t&) const
{
	throw std::runtime_error("cannot compare array and string");
}
bool binary_op_helper::operator()(const east::array_v& l, const east::array_v& r) const
{
	//TODO: implement in for arraies
	if(cmp_==comparator::in) throw std::logic_error("not ready yet!");
	bool size_ok = default_cmp(l.items.size(), r.items.size());
	return size_ok;
}
bool binary_op_helper::operator()(const east::array_v&, const east::map_v&) const
{
	throw std::runtime_error("cannot compare array and map");
}

bool binary_op_helper::operator()(const east::map_v&, const bool&) const
{
	throw std::logic_error("not ready yet!");
}
bool binary_op_helper::operator()(const east::map_v&, const double&) const
{
	throw std::logic_error("not ready yet!");
}
bool binary_op_helper::operator()(const east::map_v&, const east::string_t&) const
{
	throw std::logic_error("not ready yet!");
}
bool binary_op_helper::operator()(const east::map_v&, const east::array_v&) const
{
	throw std::logic_error("not ready yet!");
}
bool binary_op_helper::operator()(const east::map_v&, const east::map_v&) const
{
	throw std::logic_error("not ready yet!");
}
