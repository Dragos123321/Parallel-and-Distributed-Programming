#include "graph.h"

#include <random>

Graph::Graph(int nr_nodes)
{
    for (int i = 0; i < nr_nodes; ++i) {
        m_nodes.push_back(i);
        m_neighbours[i] = std::set<int>();
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, nr_nodes - 1);

    for (int i = 0; i < 2 * nr_nodes; ++i) {
        auto node1 = dist(dev);
        auto node2 = dist(dev);

        while (node1 == node2) {
            node2 = dist(dev);
        }

        m_neighbours[node1].insert(node2);
        m_neighbours[node2].insert(node1);
    }
    printf("Finished generating\n");
}

void Graph::addEdge(int node1, int node2)
{
    m_neighbours[node1].insert(node2);
    m_neighbours[node2].insert(node1);
}

bool Graph::isEdge(int node1, int node2)
{
    return std::find(m_neighbours[node1].begin(), m_neighbours[node1].end(), node2) != m_neighbours[node1].end();
}

const std::vector<int> Graph::nodes() const
{
    return m_nodes;
}

const std::set<int> Graph::neighbours(int node)
{
    return m_neighbours[node];
}
