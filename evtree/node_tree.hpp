/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <algorithm>

namespace cppjinja::evt {

namespace node_tree_details {
void throw_root_exists() ;
void throw_root_not_exists() ;
} // namespace node_tree_details

template<typename Node>
struct edge {
	edge(Node* p, Node* c) : parent(p), child(c) {}
	Node* parent;
	Node* child;
};

template<typename Node>
class node_tree {
	std::pmr::vector<Node*> all_roots;
	std::pmr::vector<edge<Node>> edges;

	void require_no_root(Node* r)
	{
		auto pos = std::find(all_roots.begin(), all_roots.end(), r);
		if(pos!=all_roots.end())
			node_tree_details::throw_root_exists();
	}
	void require_parent(Node* r)
	{
		for(auto& e:edges) if(e.child == r) return ;
		for(auto& e:all_roots) if(e == r) return ;
		node_tree_details::throw_root_not_exists();
	}
public:
	void add_root(Node* n)
	{
		require_no_root(n);
		all_roots.emplace_back(n);
	}

	void add_child(Node* parent, Node* child)
	{
		require_parent(parent);
		edges.emplace_back(parent, child);
	}

	std::pmr::vector<Node*> children(Node* parent) const
	{
		std::pmr::vector<Node*> ret;
		for(auto& edge:edges)
			if(edge.parent == parent)
				ret.emplace_back(edge.child);
		return ret;
	}

	std::pmr::vector<Node*> roots() const
	{
		std::pmr::vector<Node*> ret;
		ret.reserve(all_roots.size());
		for(auto& v:all_roots) ret.emplace_back(v);
		return ret;
	}

	std::pmr::vector<edge<Node>> all_tree() const
	{
		return edges;
	}
};

} // namespace cppjinja::evt
