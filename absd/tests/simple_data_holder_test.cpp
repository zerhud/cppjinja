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
#include "absd/simple_data_holder.hpp"

BOOST_AUTO_TEST_SUITE(absd)
using namespace absd::literals;

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
}
BOOST_AUTO_TEST_CASE(simple_double)
{
	simple_dh hld;
	hld = 2.45;
	check_reflect(hld.reflect(), absd::data_type::floating_point, 0, 0);
	BOOST_TEST(hld.to_double() == 2.45);
}
BOOST_AUTO_TEST_CASE(simple_bool)
{
	simple_dh hld;
	hld = true;
	check_reflect(hld.reflect(), absd::data_type::boolean, 0, 0);
	BOOST_TEST(hld.to_bool() == true);
}
BOOST_AUTO_TEST_CASE(simple_string)
{
	simple_dh hld, hld2;
	const simple_dh& chld = hld;
	hld.str() = "kuku";
	check_reflect(hld.reflect(), absd::data_type::string, 0, 0);
	BOOST_TEST(hld.to_string() == "kuku");
	BOOST_TEST(chld.str().get_allocator().resource() == hld.storage());
	hld2 = "test";
	BOOST_TEST(hld2.to_string() == "test");
}
BOOST_AUTO_TEST_CASE(object_at_ctor)
{
	std::pmr::map<std::pmr::string,absd::data> obj;
	obj.emplace(std::make_pair("key", 1_sd));
	simple_dh hld = obj;
	BOOST_TEST(hld.by_key("key") == 1);
}
BOOST_AUTO_TEST_CASE(object)
{
	simple_dh hld;
	BOOST_CHECK_THROW(hld.by_key("t1"), std::exception);
	BOOST_CHECK_THROW(hld.at_key("t1"), std::exception);
	hld.put("t1") = 42;
	check_reflect(hld.reflect(), absd::data_type::object, 0, 1);
	BOOST_TEST(hld.reflect().keys.at(0) == "t1");
	BOOST_TEST(hld.by_key("t1")->to_int() == 42);
	BOOST_TEST(hld.at_key("t1")->to_int() == 42);
	check_reflect(hld.by_key("no")->reflect(), absd::data_type::empty, 0, 0);
	BOOST_CHECK_THROW(hld.at_key("no"), std::out_of_range);
}
BOOST_AUTO_TEST_CASE(empty_object)
{
	auto hld = std::make_shared<simple_dh>();
	hld->make_empty_object();
	check_reflect(hld->reflect(), absd::data_type::object, 0, 0);
	std::stringstream out;
	out << absd::data{hld};
	BOOST_TEST(out.str() == "{}");
}
BOOST_AUTO_TEST_CASE(object_with_data)
{
	simple_dh hld;
	auto val_dh = std::make_shared<simple_dh>();
	hld.put("t1", absd::data{val_dh});
	BOOST_TEST(hld.by_key("t1") == val_dh);
}
BOOST_AUTO_TEST_CASE(array_at_ctor)
{
	std::pmr::vector<absd::data> ar;
	ar.emplace_back(1_sd);
	ar.emplace_back(2_sd);
	simple_dh hld = ar;
	BOOST_TEST(hld.by_ind(0)->to_int() == 1);
	BOOST_TEST(hld.by_ind(1)->to_int() == 2);
}
BOOST_AUTO_TEST_CASE(array)
{
	simple_dh hld;
	BOOST_CHECK_THROW(hld.by_ind(0), std::exception);
	BOOST_CHECK_THROW(hld.at_ind(0), std::exception);
	hld.push_back() = 42;
	check_reflect(hld.reflect(), absd::data_type::array, 1, 0);
	BOOST_TEST(hld.by_ind(0)->to_int() == 42);
	BOOST_TEST(hld.at_ind(0)->to_int() == 42);
	check_reflect(hld.by_ind(1)->reflect(), absd::data_type::empty, 0, 0);
	BOOST_CHECK_THROW(hld.at_ind(1), std::out_of_range);
	hld.push_back() = 43;
	BOOST_TEST(hld.by_ind(1)->to_int() == 43);
	BOOST_TEST(hld.at_ind(1)->to_int() == 43);
}
BOOST_AUTO_TEST_CASE(empty_array)
{
	auto hld = std::make_shared<simple_dh>();
	hld->make_empty_array();
	check_reflect(hld->reflect(), absd::data_type::array, 0, 0);
	std::stringstream out;
	out << absd::data{hld};
	BOOST_TEST(out.str() == "[]");
}
BOOST_AUTO_TEST_CASE(array_with_data)
{
	simple_dh hld;
	auto val_dh = std::make_shared<simple_dh>();
	hld.push_back(data{val_dh});
	BOOST_TEST(hld.by_ind(0) == val_dh);
}
BOOST_AUTO_TEST_CASE(change)
{
	simple_dh hld;
	hld = 10;
	BOOST_TEST(hld.reflect().type == absd::data_type::integer);
	hld = false;
	BOOST_TEST(hld.reflect().type == absd::data_type::boolean);
	BOOST_TEST(hld.to_bool() == false);

	auto val_dh = std::make_shared<simple_dh>();
	hld.put("t1", absd::data{val_dh});
	BOOST_TEST(hld.reflect().type == absd::data_type::object);

	hld = 42;
	BOOST_TEST(hld.reflect().type == absd::data_type::integer);

	hld.push_back() = 43;
	BOOST_TEST(hld.reflect().type == absd::data_type::array);

	hld = 44;
	BOOST_TEST(hld.reflect().type == absd::data_type::integer);
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
	BOOST_TEST(hld_obj.by_key("t1")->to_int() == 42);
	BOOST_TEST(hld_ar.by_ind(0)->to_int() == 42);
}
BOOST_AUTO_TEST_SUITE_END() // simple_data_holder_tests
BOOST_AUTO_TEST_SUITE_END() // absd
