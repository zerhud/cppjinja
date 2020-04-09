/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context.hpp"

cppjinja::evt::raii_push_ctx::raii_push_ctx(
        const cppjinja::evt::node* n, cppjinja::evt::context* c)
    : ctx(c)
    , maker(n)
{
	ctx->push(maker);
}

cppjinja::evt::raii_push_ctx::~raii_push_ctx()
{
	ctx->pop(maker);
}

cppjinja::evt::raii_inject::raii_inject(
        const evtnodes::callable* n, cppjinja::evt::context* c)
    : ctx(c)
    , maker(n)
{
	ctx->inject(maker);
}

cppjinja::evt::raii_inject::~raii_inject()
{
	ctx->takeout(maker);
}
