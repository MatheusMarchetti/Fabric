#include "utilities.hpp"
#include "source/common/common_headers.hpp"
#include "source/common/identifier.hpp"

namespace fabric::utl
{
	class graph
	{
		struct graph_node
		{
			id::id_type node_id;
			u32 node_index;
			u32 queue_index = 0;
			u32 global_execution_index = 0;
			u32 dependency_level_index = 0;
			u32 local_dependency_level_index = 0;
			u32 local_queue_index = 0;
			utl::unordered_set<u32> dependents;
			utl::vector<u32> synchronization_index_set;
			utl::vector<const graph_node*> nodes_to_sync;
		};

		struct dependency_level
		{
			u32 level_index = 0;
			utl::list<graph_node*> nodes;
			utl::vector<utl::vector<const graph_node*>> queue_nodes;
			utl::unordered_set<u32> cross_read_queues;
			utl::unordered_set<id::id_type> multi_queue_nodes;
		};

		struct sync_coverage
		{
			const graph_node* sync_node = nullptr;
			u32 sync_node_index = 0;
			utl::vector<u32> synced_queue_indices;
		};

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

		utl::vector<utl::vector<id::id_type>> get_execution_order();

		void clear();

	private:
		bool sort();

		void build_adjacency_list();

		void build_dependency_levels();

		void build_synchronizations();

		void cull_redundant_synchronizations();

		void depth_first_search(u32 node_index, std::vector<bool>& visited, std::vector<bool>& stack, bool& cyclic);

	private:
		u32 m_queue_count = 1;
		utl::unordered_map<id::id_type, graph_node> m_nodes_map;
		utl::vector<graph_node> m_nodes;
		utl::vector<utl::vector<u32>> m_adjacency_list;
		utl::vector<dependency_level> m_dependency_levels;
		utl::unordered_map<u32, u32> m_queue_node_counters;
		utl::vector<graph_node*> m_sorted_nodes;
		utl::vector<graph_node*> m_global_execution_nodes;
		utl::vector<utl::vector<const graph_node*>> m_queue_nodes;
	};

	inline graph::graph(u32 size)
	{
		resize(size);
	}

	inline graph::~graph()
	{
		clear();
	}

	inline graph::graph(graph& other)
	{
		resize((u32)other.m_nodes.size());

		m_nodes_map.swap(other.m_nodes_map);
		m_nodes.swap(other.m_nodes);
		m_adjacency_list.swap(other.m_adjacency_list);
		m_dependency_levels.swap(other.m_dependency_levels);
		m_queue_node_counters.swap(other.m_queue_node_counters);
		m_sorted_nodes.swap(other.m_sorted_nodes);
		m_global_execution_nodes.swap(other.m_global_execution_nodes);
		m_queue_nodes.swap(other.m_queue_nodes);
	}

	inline graph& graph::operator=(graph& other)
	{
		resize((u32)other.m_nodes.size());

		m_nodes_map.swap(other.m_nodes_map);
		m_nodes.swap(other.m_nodes);
		m_adjacency_list.swap(other.m_adjacency_list);
		m_dependency_levels.swap(other.m_dependency_levels);
		m_queue_node_counters.swap(other.m_queue_node_counters);
		m_sorted_nodes.swap(other.m_sorted_nodes);
		m_global_execution_nodes.swap(other.m_global_execution_nodes);
		m_queue_nodes.swap(other.m_queue_nodes);

		return *this;
	}

	inline graph graph::create(u32 size)
	{
		return graph(size);
	}

	inline void graph::add_node(id::id_type node)
	{
		if (!m_nodes_map.contains(node))
		{
			graph_node g_node
			{
				.node_id = node
			};

			m_nodes.push_back(g_node);
			m_nodes.back().node_index = (u32)m_nodes.size() - 1;
			m_nodes_map.insert({ node, m_nodes.back() });
		}
	}

	inline void graph::add_dependency(id::id_type dependent, id::id_type dependency)
	{
		assert(m_nodes_map.contains(dependent));

		graph_node& g_node = m_nodes_map[dependent];

		add_node(dependency);

		u32 index = m_nodes_map[dependency].node_index;
		graph_node& d_node = m_nodes[index];
		d_node.dependents.insert(g_node.node_index);

		m_nodes_map[dependency] = d_node;
	}

	inline void graph::resize(u32 new_size)
	{
		m_adjacency_list.resize(new_size);
		m_sorted_nodes.reserve(new_size);
		m_global_execution_nodes.reserve(new_size);
	}

	inline void graph::build()
	{
		if(m_nodes_map.size() != m_global_execution_nodes.size())
		{
			clear();

			build_adjacency_list();

			if (sort())
			{
				build_dependency_levels();
				build_synchronizations();
				cull_redundant_synchronizations();
			}
		}
	}

	inline void graph::build_adjacency_list()
	{
		m_adjacency_list.resize(m_nodes.size());

		for (u32 node_index = 0; node_index < m_nodes.size(); node_index++)
		{
			graph_node& node = m_nodes[node_index];

			if (node.dependents.empty())
				continue;

			utl::vector<u32>& adjacent_node_indices = m_adjacency_list[node_index];

			for (auto& other_index : node.dependents)
			{
				graph_node& other_node = m_nodes[other_index];
				adjacent_node_indices.push_back(other_index);
				if (node.queue_index != other_node.queue_index)
					node.nodes_to_sync.push_back(&other_node);
			}
		}
	}

	inline bool graph::sort()
	{
		utl::vector<bool> visited_nodes(m_nodes.size(), false);
		utl::vector<bool> on_stack(m_nodes.size(), false);

		bool is_cyclic = false;

		for (u32 node_index = 0; node_index < m_nodes.size(); node_index++)
		{
			const graph_node& node = m_nodes[node_index];

			if (!visited_nodes[node_index] && !node.dependents.empty())
				depth_first_search(node_index, visited_nodes, on_stack, is_cyclic);
		}

		std::reverse(m_sorted_nodes.begin(), m_sorted_nodes.end());

		return !is_cyclic;
	}

	inline void graph::build_dependency_levels()
	{
		utl::vector<u32> longest_distances(m_nodes.size(), 0);
		
		u32 dependency_level_count = 1;

		for (u32 node_index = 0; node_index < m_sorted_nodes.size(); node_index++)
		{
			u32 original_index = m_sorted_nodes[node_index]->node_index;
			u32 adjacency_list_index = original_index;

			for (u32 adjacent_node_index : m_adjacency_list[adjacency_list_index])
			{
				if (longest_distances[adjacent_node_index] < longest_distances[original_index] + 1)
				{
					u32 new_longest_distance = longest_distances[original_index] + 1;
					longest_distances[adjacent_node_index] = new_longest_distance;

					dependency_level_count = std::max(new_longest_distance + 1, dependency_level_count);
				}
			}
		}

		m_dependency_levels.resize(dependency_level_count);
		m_queue_count = 1;

		for (u32 node_index = 0; node_index < m_sorted_nodes.size(); node_index++)
		{
			graph_node* node = m_sorted_nodes[node_index];

			u32 level_index = longest_distances[node->node_index];

			dependency_level& level = m_dependency_levels[level_index];
			level.level_index = level_index;
			level.nodes.push_back(node);

			node->dependency_level_index = level_index;

			m_queue_count = std::max(m_queue_count, node->queue_index + 1);
		}
	}

	inline void graph::depth_first_search(u32 node_index, std::vector<bool>& visited, std::vector<bool>& stack, bool& cyclic)
	{
		if (cyclic)
			return;

		visited[node_index] = true;
		stack[node_index] = true;

		u32 adjacency_list_index = m_nodes[node_index].node_index;

		for (u32 neighbour : m_adjacency_list[adjacency_list_index])
		{
			if (visited[neighbour] && stack[neighbour])
			{
				cyclic = true;
				return;
			}

			if (!visited[neighbour])
			{
				depth_first_search(neighbour, visited, stack, cyclic);
			}
		}

		stack[node_index] = false;
		m_sorted_nodes.push_back(&m_nodes[node_index]);
	}

	inline utl::vector<utl::vector<id::id_type>> graph::get_execution_order()
	{
		utl::vector<utl::vector<id::id_type>> execution_list(m_dependency_levels.size());

		for (auto& level : m_dependency_levels)
		{
			for (auto node : level.nodes)
			{
				execution_list[level.level_index].push_back(node->node_id);
			}
		}

		return execution_list;
	}

	void graph::build_synchronizations()
	{
		u32 global_execution_index = 0;

		m_global_execution_nodes.resize(m_sorted_nodes.size(), nullptr);
		m_queue_nodes.resize(m_queue_count);

		utl::vector<const graph_node*> queue_previous_nodes(m_queue_count, nullptr);

		for (dependency_level& dependency_level : m_dependency_levels)
		{
			u32 local_execution_index = 0;

			utl::unordered_map<u32, utl::unordered_set<u32>> dependency_queue_tracker;

			dependency_level.queue_nodes.resize(m_queue_count);

			for (graph_node* node : dependency_level.nodes)
			{
				for (u32 dependent : node->dependents)
				{
					dependency_queue_tracker[dependent].insert(node->queue_index);
				}

				node->global_execution_index = global_execution_index;
				node->local_dependency_level_index = local_execution_index;
				node->local_queue_index = m_queue_node_counters[node->queue_index]++;

				m_global_execution_nodes[global_execution_index] = node;

				dependency_level.queue_nodes[node->queue_index].push_back(node);

				m_queue_nodes[node->queue_index].push_back(node);

				if (queue_previous_nodes[node->queue_index])
				{
					node->nodes_to_sync.push_back(queue_previous_nodes[node->queue_index]);
				}

				queue_previous_nodes[node->queue_index] = node;

				local_execution_index++;
				global_execution_index++;
			}

			for (auto& [dependent, queue_indices] : dependency_queue_tracker)
			{
				if (queue_indices.size() > 1)
				{
					for (u32 queue_index : queue_indices)
					{
						dependency_level.cross_read_queues.insert(queue_index);
						dependency_level.multi_queue_nodes.insert(dependent);
					}
				}
			}
		}
	}

	void graph::cull_redundant_synchronizations()
	{
		for (graph_node& node : m_nodes)
			node.synchronization_index_set.resize(m_queue_count, u32_invalid_id);

		for (dependency_level& dependency_level : m_dependency_levels)
		{
			for (graph_node* node : dependency_level.nodes)
			{
				utl::vector<const graph_node*> closest_sync_node(m_queue_count, nullptr);

				for (const graph_node* dependent_node : node->nodes_to_sync)
				{
					const graph_node* closest_node = closest_sync_node[dependent_node->queue_index];

					if (!closest_node || dependent_node->local_queue_index > closest_node->local_queue_index)
						closest_sync_node[dependent_node->queue_index] = dependent_node;
				}

				node->nodes_to_sync.clear();

				for (u32 queue_index = 0; queue_index < m_queue_count; queue_index++)
				{
					const graph_node* closest_node = closest_sync_node[queue_index];

					if (!closest_node)
					{
						const graph_node* previous_node = closest_sync_node[node->queue_index];

						if (previous_node)
						{
							u32 sync_index = previous_node->synchronization_index_set[queue_index];
							node->synchronization_index_set[queue_index] = sync_index;
						}
					}
					else
					{
						if (closest_node->queue_index != node->queue_index)
							node->synchronization_index_set[closest_node->queue_index] = closest_node->local_queue_index;

						node->nodes_to_sync.push_back(closest_node);
					}
				}

				node->synchronization_index_set[node->queue_index] = node->local_queue_index;
			}

			for (graph_node* node : dependency_level.nodes)
			{
				utl::unordered_set<u32> queue_sync;
				utl::vector<sync_coverage> coverage_array;
				utl::vector<const graph_node*> optimal_sync;

				for (const graph_node* sync_node : node->nodes_to_sync)
					queue_sync.insert(sync_node->queue_index);

				while (!queue_sync.empty())
				{
					u32 syncs_covered_by_single_node = 0;

					for (u32 dependency_node_index = 0; dependency_node_index < node->nodes_to_sync.size(); dependency_node_index++)
					{
						const graph_node* dependency_node = node->nodes_to_sync[dependency_node_index];

						utl::vector<u32> synced_indices;

						for (u32 queue_index : queue_sync)
						{
							u32 current_node_sync_index = node->synchronization_index_set[queue_index];
							u32 dependency_node_sync_index = dependency_node->synchronization_index_set[queue_index];

							assert(current_node_sync_index != u32_invalid_id);

							if (queue_index == node->queue_index)
								current_node_sync_index -= 1;

							if (dependency_node_sync_index != u32_invalid_id && dependency_node_sync_index >= current_node_sync_index)
								synced_indices.push_back(queue_index);
						}

						coverage_array.emplace_back(sync_coverage{ dependency_node, dependency_node_index, synced_indices });
						syncs_covered_by_single_node = std::max(syncs_covered_by_single_node, (u32)synced_indices.size());
					}

					for (const sync_coverage& sync_coverage : coverage_array)
					{
						u32 covered_sync_count = (u32)sync_coverage.synced_queue_indices.size();

						if (covered_sync_count >= syncs_covered_by_single_node)
						{
							if (sync_coverage.sync_node->queue_index != node->queue_index)
							{
								optimal_sync.push_back(sync_coverage.sync_node);

								u32& index = node->synchronization_index_set[sync_coverage.sync_node->queue_index];
								index = std::max(index, node->synchronization_index_set[sync_coverage.sync_node->queue_index]);
							}

							for (u32 synced_queue_index : sync_coverage.synced_queue_indices)
								queue_sync.erase(synced_queue_index);
						}
					}

					for (auto sync_coverage_it = coverage_array.rbegin(); sync_coverage_it != coverage_array.rend(); ++sync_coverage_it)
						if(!node->nodes_to_sync.empty())
							node->nodes_to_sync.erase(node->nodes_to_sync.begin() + sync_coverage_it->sync_node_index);
				}

				node->nodes_to_sync = optimal_sync;
			}
		}
	}

	inline void graph::clear()
	{
		m_adjacency_list.clear();
		m_dependency_levels.clear();
		m_queue_node_counters.clear();
		m_sorted_nodes.clear();
		m_global_execution_nodes.clear();
		m_queue_nodes.clear();
	}
}