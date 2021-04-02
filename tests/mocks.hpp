/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "parser/parse.hpp"
#include "parser/common.hpp"

namespace cppjinja_mocks {

MOCK_BASE_CLASS(parser_env, cppjinja::parser_env)
{
	MOCK_METHOD(file_name, 0)
	MOCK_METHOD(on_error, 0)
	MOCK_METHOD(elog, 0)
	MOCK_METHOD(format_err_msg, 1)
};

struct parser_fixture {
	std::shared_ptr<parser_env> env =
		std::make_shared<parser_env>();
	std::stringstream eout;

	parser_fixture()
	{
		using namespace std::literals;
		MOCK_EXPECT(env->elog).returns(std::ref(eout));
		MOCK_EXPECT(env->format_err_msg).calls([](auto w){return "Error! "s + w;});
	}

	void expect_fn(std::string fn)
	{
		MOCK_EXPECT(env->file_name).returns(fn);
	}
};

} // namespace cppjinja_mocks
