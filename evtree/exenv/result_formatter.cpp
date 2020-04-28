/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "result_formatter.hpp"

#include <boost/algorithm/string/replace.hpp>

void cppjinja::evt::result_formatter::shift_tab(int val)
{
	if(bval + val < 0)
		throw std::runtime_error("cannot shift_tab under zero");
	bval += val;
}

std::string cppjinja::evt::result_formatter::operator()(std::string src) const
{
	assert( 0<= bval );
	std::string tabs = "\n";
	for(int i=0;i<bval;++i) tabs += "\t";
	boost::algorithm::replace_all(src, "\n", tabs);
	return src;
}
