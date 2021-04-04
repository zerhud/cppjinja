/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <boost/json/src.hpp>
#include <boost/program_options.hpp>
#include <loader/parser.hpp>
#include <evtree/evtree.hpp>
#include "jparse/json_prov.hpp"

int main(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description opts;
	po::variables_map opt_vals;
	opts.add_options()
		("help,h", "produce the help message")
		("tmpl,t", po::value<std::string>(), "the template file")
		;
	po::store(po::command_line_parser(argc,argv).options(opts).run(), opt_vals);
	if(opt_vals.count("help") || opt_vals.count("tmpl")==0) {
		std::cout
			<< "test utility for parse jinja files"
			<< std::endl << opts << std::endl;
		std::exit(0);
	}

	cppjinja::evtree ev;
	cppjinja::parser jparser({});
	jparser.parse(opt_vals["tmpl"].as<std::string>().c_str());
	for(auto& t:jparser.tmpls()) ev.add_tmpl(t);

	std::cout << "parsed ok" << std::endl;

	std::string json_data;
	std::cin >> json_data;
	cogen::jinja_json_prov data(boost::json::parse(json_data));
	ev.render(std::cout, data, "");
	return 0;
}
