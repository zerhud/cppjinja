/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <fstream>
#include <boost/json/src.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <loader/parser.hpp>
#include <absd/simple_data_holder.hpp>
#include <evtree/evtree.hpp>
#include "jparse/json_prov.hpp"

using absd::make_simple;
using namespace absd::literals;

class extra_functions : public cogen::jinja_json_prov {
	static bool contains(const std::vector<absd::data>& con, absd::data val)
	{
		for(auto& v:con) if(v==val) return true;
		return false;
	}

	absd::data extract_name(absd::data obj, absd::data nind=make_simple(-1)) const
	{
		auto name = obj["name"];
		std::int64_t ind = (std::int64_t)nind;
		if(name.is_array()) return name[0 <= ind ? ind : 0];
		return name;
	}

	absd::data select_keys(absd::data obj, absd::data vals) const
	{
		auto ret = std::make_shared<absd::simple_data_holder>();
		auto vals_arr = vals.as_array();
		auto contains = [&vals_arr](auto&& v){for(auto& av:vals_arr) if(av==v)return true; return false;};
		if(obj.is_object()) {
			for(auto& [k,v]:obj.as_map()) if(contains(k)) {
				if(v.is_array())
					for(auto& vv:v.as_array())
						ret->push_back(vv);
				else ret->push_back(v);
			}
		}
		return absd::data{ret};
	}
	absd::data unique(absd::data vals) const
	{
		if(!vals.is_array()) return vals;
		std::vector<absd::data> result;
		for(auto& v:vals.as_array())
			if(!contains(result, v)) result.emplace_back(v);
		auto ret = std::make_shared<absd::simple_data_holder>();
		for(auto& r:result) ret->push_back(std::move(r));
		return absd::data{ret};
	}
	absd::data apply_prefix(absd::data obj, absd::data prefix, absd::data nind=0_sd) const
	{
		auto naming = (obj["naming"][(std::int64_t)nind]).str();
		if(naming=="underscore") return make_simple(prefix.str() + "_" + extract_name(obj, nind).str());
		if(naming=="title_case") return make_simple(prefix.str() + extract_name(obj, nind).str());
		if(naming=="camel_case") return make_simple(prefix.str() + "_" + extract_name(obj, nind).str());
		return absd::make_simple(prefix.str() + extract_name(obj, nind).str());
	}
	absd::data apply_sufix(absd::data obj, absd::data prefix, absd::data nind=0_sd) const
	{
		auto naming = (obj["naming"][(std::int64_t)nind]).str();
		if(naming=="underscore") return make_simple(prefix.str() + "_" + extract_name(obj, nind).str());
		if(naming=="title_case") return make_simple(prefix.str() + extract_name(obj, nind).str());
		if(naming=="camel_case") return make_simple(prefix.str() + "_" + extract_name(obj, nind).str());
		return absd::make_simple(extract_name(obj, nind).str() + prefix.str());
	}
public:
	extra_functions(boost::json::value jd) : jinja_json_prov(std::move(jd)) {}
	absd::data value(const cppjinja::east::function_call& val) const override
	{
		if(val.ref.size()!=1) return jinja_json_prov::value(std::move(val));
		if(val.ref[0] == "select_keys") {
			if(val.params.size() != 2)
				throw std::runtime_error("function select_keys takes two arguments");
			return select_keys(
			            std::move(val.params[0].val.value()),
			            std::move(val.params[1].val.value()));
		} else if(val.ref[0] == "unique") {
			if(val.params.size() != 1)
				throw std::runtime_error("function unique takes one argument");
			return unique( std::move(val.params[0].val.value()) );
		} else if(val.ref[0] == "apply_prefix") {
			if(val.params.size()==2)
				return apply_prefix(val.params[0].val.value(), val.params[1].val.value());
			if(val.params.size()==3)
				return apply_prefix(val.params[0].val.value(), val.params[1].val.value(), val.params[2].val.value());
			throw std::runtime_error("function apply_prefix takes 2 or 3 arguments");
		} else if(val.ref[0] == "apply_sufix") {
			if(val.params.size()==2)
				return apply_prefix(val.params[0].val.value(), val.params[1].val.value());
			if(val.params.size()==3)
				return apply_sufix(val.params[0].val.value(), val.params[1].val.value(), val.params[2].val.value());
			throw std::runtime_error("function apply_sufix takes 2 or 3 arguments");
		} else if(val.ref[0] == "name") {
			if(val.params.size()==2)
				return extract_name(val.params[0].val.value(), val.params[1].val.value());
			if(val.params.size()==1)
				return extract_name(val.params[0].val.value());
			throw std::runtime_error("function extract_name takes 1 or 2 arguments");
		}
		return jinja_json_prov::value(std::move(val));
	}
};

int main(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description opts;
	po::variables_map opt_vals;
	opts.add_options()
		("help,h", "produce the help message")
		("tmpl,t", po::value<std::string>(), "the template file")
		("data,i", po::value<std::string>()->default_value(""), "the data file")
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
	std::filesystem::path rtmpl = opt_vals["tmpl"].as<std::string>();
	rtmpl = absolute(rtmpl);
	jparser.parse(rtmpl);
	for(auto& t:jparser.tmpls()) ev.add_tmpl(t);

	std::cout << "parsed ok" << std::endl;

	std::string json_data, json_line;
	if(opt_vals["data"].as<std::string>() == "")
		while(std::getline(std::cin, json_line)) json_data += json_line;
	else {
		std::fstream data(opt_vals["data"].as<std::string>(), std::ios::in);
		while(std::getline(data, json_line)) json_data += json_line;
	}

	extra_functions data(boost::json::parse(json_data));
	ev.render(std::cout, data, rtmpl.generic_string());
	return 0;
}
