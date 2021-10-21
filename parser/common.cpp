/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"
#include <iostream>

cppjinja::throw_cerr_env::throw_cerr_env(std::string_view file_name)
    : cur_file(file_name)
{
}

std::string cppjinja::throw_cerr_env::file_name() const
{
	return cur_file;
}

void cppjinja::throw_cerr_env::on_error() const
{
	throw std::runtime_error(
	            "cannot parse " +
	            std::string(cur_file.begin(),cur_file.end()));
}

std::ostream& cppjinja::throw_cerr_env::elog() const
{
	return std::cerr;
}

std::string cppjinja::throw_cerr_env::format_err_msg(const std::string& which_rule) const
{
	return "Error! Got error in " + which_rule;
}

const cppjinja::parser_env* cppjinja::parser_env::default_env()
{
	static throw_cerr_env env{""};
	return &env;
}
