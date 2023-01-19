#include "graph.h"

Graph::Graph(int nr_nodes)
{
    for (int i = 0; i < nr_nodes; ++i) {
        m_nodes.push_back(i);
        m_neighbours[i] = std::set<int>();
    }
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
