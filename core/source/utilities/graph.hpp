#include "utilities.hpp"
#include "source/common/common_headers.hpp"
#include "source/common/identifier.hpp"

namespace fabric::utl
{
	class graph
	{
	public:
		graph() = default;

		graph(u32 size);

		~graph();

		graph(graph& other);

		graph& operator=(graph& other);

		static graph create(u32 size);

		void add_node(id::id_type node);

		void add_dependency(id::id_type node, id::id_type dependency);

		void resize(u32 new_size);

		void build();

		bool sort();

		void build_dependency_levels();

		utl::vector<utl::vector<id::id_type>> get_execution_order();

		void clear();

	private:
		struct graph_node
		{
			id::id_type node_id;
			u32 node_index;
			u32 dependency_level_index = u32_invalid_id;
			u32 queue_index = 0;
		};

		struct dependency_level
		{
			u32 level_index;
			utl::vector<graph_node> level_nodes;
		};

		void depth_first_search(graph_node& node, std::vector<bool>& visited, std::vector<bool>& stack, bool& cyclic);

	private:
		u32 m_node_index;
		u32 m_queue_count;
		utl::unordered_map<id::id_type, graph_node> m_nodes;
		utl::vector<utl::vector<graph_node>> m_adjacency_list;
		utl::vector<graph_node> m_node_list;
		utl::vector<dependency_level> m_dependency_levels;
	};

	inline graph::graph(u32 size)
		: m_node_index(0)
	{
		resize(size);
	}

	inline graph::~graph()
	{
		m_nodes.clear();
		m_adjacency_list.clear();
		m_node_list.clear();
	}

	inline graph::graph(graph& other)
	{
		m_node_index = other.m_node_index;

		resize((u32)other.m_nodes.size());

		m_nodes.swap(other.m_nodes);

		m_adjacency_list.swap(other.m_adjacency_list);

		m_node_list.swap(other.m_node_list);
	}

	inline graph& graph::operator=(graph& other)
	{
		m_node_index = other.m_node_index;

		resize((u32)other.m_nodes.size());

		m_nodes.swap(other.m_nodes);

		m_adjacency_list.swap(other.m_adjacency_list);

		m_node_list.swap(other.m_node_list);

		return *this;
	}

	inline graph graph::create(u32 size)
	{
		return graph(size);
	}

	inline void graph::add_node(id::id_type node)
	{
		if (!m_nodes.contains(node))
		{
			graph_node g_node
			{
				.node_id = node,
				.node_index = m_node_index++
			};

			m_nodes[node] = g_node;
		}
	}

	inline void graph::add_dependency(id::id_type node, id::id_type dependency)
	{
		graph_node& g_node = m_nodes[node];

		if (!m_nodes.contains(dependency))
		{
			graph_node d_node
			{
				.node_id = dependency,
				.node_index = m_node_index++
			};

			m_nodes[dependency] = d_node;
		}

		if (m_adjacency_list.size() != m_nodes.size())
			resize((u32)m_nodes.size());

		m_adjacency_list[m_nodes[dependency].node_index].push_back(g_node);
	}

	inline void graph::resize(u32 new_size)
	{
		m_adjacency_list.resize(new_size);
		m_node_list.reserve(new_size);
	}

	inline void graph::build()
	{
		if (sort())
		{
			build_dependency_levels();
		}
	}

	inline bool graph::sort()
	{
		utl::vector<bool> visited_nodes(m_nodes.size(), false);
		utl::vector<bool> on_stack(m_nodes.size(), false);

		bool is_cyclic = false;

		for (auto node_it = m_nodes.begin(); node_it != m_nodes.end(); node_it++)
		{
			graph_node& node = node_it->second;
			
			if (!visited_nodes[node.node_index] && !m_adjacency_list[node.node_index].empty())
			{
				depth_first_search(node, visited_nodes, on_stack, is_cyclic);

				assert(!is_cyclic);
			}
		}

		std::reverse(m_node_list.begin(), m_node_list.end());

		return !is_cyclic;
	}

	inline void graph::build_dependency_levels()
	{
		utl::vector<u32> longest_distances(m_nodes.size(), 0);
		
		u32 dependency_level_count = 1;

		for (u32 node_index = 0; node_index < m_node_list.size(); node_index++)
		{
			u32 original_index = m_node_list[node_index].node_index;

			for (graph_node& node : m_adjacency_list[original_index])
			{
				if (longest_distances[node.node_index] < longest_distances[original_index])
				{
					u32 new_longest_distance = longest_distances[original_index] + 1;

					longest_distances[node.node_index] = new_longest_distance;

					dependency_level_count = std::max(new_longest_distance + 1, dependency_level_count);
				}
			}
		}

		m_dependency_levels.resize(dependency_level_count);
		m_queue_count = 1;

		for (u32 node_index = 0; node_index < m_node_list.size(); node_index++)
		{
			graph_node& node = m_node_list[node_index];

			u32 level_index = longest_distances[node.node_index];

			node.dependency_level_index = level_index;

			dependency_level& level = m_dependency_levels[level_index];
			level.level_index = level_index;
			level.level_nodes.push_back(node);

			m_queue_count = std::max(m_queue_count, node.queue_index + 1);
		}
	}

	inline void graph::depth_first_search(graph_node& node, std::vector<bool>& visited, std::vector<bool>& stack, bool& cyclic)
	{
		if (cyclic)
			return;

		visited[node.node_index] = true;
		stack[node.node_index] = true;

		for (graph_node& neighbour : m_adjacency_list[node.node_index])
		{
			if (visited[neighbour.node_index] && stack[neighbour.node_index])
			{
				cyclic = true;
				return;
			}

			if (!visited[neighbour.node_index])
			{
				depth_first_search(neighbour, visited, stack, cyclic);
			}
		}

		stack[node.node_index] = false;
		m_node_list.push_back(node);
	}

	inline utl::vector<utl::vector<id::id_type>> graph::get_execution_order()
	{
		utl::vector<utl::vector<id::id_type>> execution_list(m_dependency_levels.size());

		for (auto& level : m_dependency_levels)
		{
			for (auto& node : level.level_nodes)
			{
				execution_list[level.level_index].push_back(node.node_id);
			}
		}

		return execution_list;
	}

	inline void graph::clear()
	{
		m_nodes.clear();
		m_adjacency_list.clear();
		m_node_list.clear();
		m_dependency_levels.clear();
	}
}