/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator nodes

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "evtree/node.hpp"
#include "evtree/nodes/tmpl.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using evt_node = cppjinja::evt::node;

boost::ptr_vector<evt_node> all_nodes()
{
	using namespace cppjinja::evtnodes;
	return {
	};
}

BOOST_AUTO_TEST_SUITE(nodes)
BOOST_AUTO_TEST_SUITE_END() // blocks
