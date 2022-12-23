#include "DirectedGraph.h"

#include <random>
#include <algorithm>
#include <string>

DirectedGraph::DirectedGraph(size_t nr_nodes)
{
    for (size_t i = 0; i < nr_nodes; ++i) {
        m_nodes.push_back(i);
        m_neighbours.push_back(std::vector<int>());
    }

    generateRandomHamiltonian();
}

int DirectedGraph::size() const
{
    return m_nodes.size();
}

const std::vector<int>& DirectedGraph::neighbours(int node) const
{
    try {
        return m_neighbours.at(node);
    }
    catch (...) {
        std::cout << "Index out of range: " << node << '\n';
    }
}

const std::vector<int>& DirectedGraph::nodes() const
{
    return m_nodes;
}

void DirectedGraph::generateRandomHamiltonian()
{
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, m_nodes.size() - 1);

    auto shuffle_nodes = m_nodes;

    std::shuffle(shuffle_nodes.begin(), shuffle_nodes.end(), gen);

    for (size_t i = 0; i < shuffle_nodes.size() - 1; ++i) {
        m_neighbours[shuffle_nodes[i]].push_back(shuffle_nodes[i + 1]);
    }

    m_neighbours[shuffle_nodes[shuffle_nodes.size() - 1]].push_back(shuffle_nodes[0]);

    for (size_t i = 0; i < shuffle_nodes.size() / 2; ++i) {
        int from = distr(gen);
        int to = distr(gen);

        while (from == to) {
            to = distr(gen);
        }

        m_neighbours[from].push_back(to);
    }
}

std::ostream& operator<<(std::ostream& out, const DirectedGraph& graph)
{   
    out << "Graph: \n";

    for (auto node : graph.nodes()) {
        out << node << ' ';
    }
    out << '\n';

    for (auto node : graph.nodes()) {
        for (auto neigh : graph.neighbours(node)) {
            out << node << " -> " << neigh << '\n';
        }
    }

    return out;
}
