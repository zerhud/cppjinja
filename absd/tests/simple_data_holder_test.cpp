/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE absd_simple_holder

#include <variant>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "simple_data_holder.hpp"

BOOST_AUTO_TEST_SUITE(absd)
using absd_mocks::fixture;
using abs_data = absd::data;
using simple_dh = absd::simple_data_holder;
BOOST_AUTO_TEST_SUITE(simple_data_holder_tests)
void check_reflect(
        const absd::reflection_info& ri,
        absd::data_type t,
        std::size_t is,
        std::size_t ks)
{
	BOOST_TEST(ri.type == t);
	BOOST_TEST(ri.size == is);
	BOOST_TEST(ri.keys.size() == ks);
}
BOOST_AUTO_TEST_CASE(simple_int)
{
	simple_dh hld;
	check_reflect(hld.reflect(), absd::data_type::empty, 0, 0);
	hld = 10;
	check_reflect(hld.reflect(), absd::data_type::integer, 0, 0);
	BOOST_TEST(hld.to_int() == 10);
	BOOST_CHECK_THROW(hld = 3.14, std::exception);
}
BOOST_AUTO_TEST_CASE(simple_double)
{
	simple_dh hld;
	hld = 2.45;
	check_reflect(hld.reflect(), absd::data_type::floating_point, 0, 0);
	BOOST_TEST(hld.to_double() == 2.45);
	BOOST_CHECK_THROW(hld = false, std::exception);
}
BOOST_AUTO_TEST_CASE(simple_bool)
{
	simple_dh hld;
	hld = true;
	check_reflect(hld.reflect(), absd::data_type::boolean, 0, 0);
	BOOST_TEST(hld.to_bool() == true);
	BOOST_CHECK_THROW(hld = false, std::exception);
	BOOST_CHECK_THROW(hld = 3.14, std::exception);
}
BOOST_AUTO_TEST_CASE(simple_string)
{
	simple_dh hld;
	const simple_dh& chld = hld;
	hld.str() = "kuku";
	check_reflect(hld.reflect(), absd::data_type::string, 0, 0);
	BOOST_TEST(hld.to_string() == "kuku");
	BOOST_CHECK_THROW(hld = false, std::exception);
	BOOST_TEST(chld.str().get_allocator().resource() == &hld.storage());
}
BOOST_AUTO_TEST_CASE(object)
{
	simple_dh hld;
	BOOST_CHECK_THROW(hld.by_key("t1"), std::exception);
	hld.put("t1") = 42;
	check_reflect(hld.reflect(), absd::data_type::object, 0, 1);
	BOOST_CHECK_THROW(hld = false, std::exception);
	BOOST_TEST(hld.reflect().keys.at(0) == "t1");
	BOOST_TEST(hld.by_key("t1")->to_int() == 42);
	BOOST_CHECK_THROW(hld.by_key("no"), std::out_of_range);
}
BOOST_AUTO_TEST_CASE(array)
{
	simple_dh hld;
	BOOST_CHECK_THROW(hld.by_ind(0), std::exception);
	hld.push_back() = 42;
	check_reflect(hld.reflect(), absd::data_type::array, 1, 0);
	BOOST_CHECK_THROW(hld = false, std::exception);
	BOOST_TEST(hld.by_ind(0)->to_int() == 42);
	BOOST_CHECK_THROW(hld.by_ind(1), std::out_of_range);
}
BOOST_AUTO_TEST_CASE(use_resource)
{
	typedef std::pmr::unsynchronized_pool_resource mem_t;
	mem_t mem;
	auto mem_ptr = std::shared_ptr<mem_t>(&mem, [](auto){});

	struct raii {
		raii() {
			std::pmr::set_default_resource(std::pmr::null_memory_resource());
		}
		~raii() {
			std::pmr::set_default_resource(std::pmr::new_delete_resource());
		}
	} mr_setter;

	simple_dh hld_obj(mem_ptr), hld_ar(mem_ptr);
	BOOST_CHECK_NO_THROW(hld_obj.put("t1") = 42);
	BOOST_CHECK_NO_THROW(hld_ar.push_back() = 42);
}
BOOST_AUTO_TEST_SUITE_END() // simple_data_holder_tests
BOOST_AUTO_TEST_SUITE_END() // absd
