/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

// this file must to be included in some namespace.
// before include command a gram_traits must to be defined.
// also all boost's includes and ast.hpp must to be included.

auto add_block_content = [](auto& ctx) {
};


const x3::rule<struct block, gram_traits::block_t(std::string,std::string)> block = "block";
auto auto block_def =
	*(
		gram_traits::char_ [ add_block_content ]
	);


//const x3::rule<struct out_string, gram_traits::out_string_t()> string = "string";
//const auto string_def = x3::lexeme[ *



