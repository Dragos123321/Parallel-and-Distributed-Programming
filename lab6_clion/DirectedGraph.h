#pragma once

#include <vector>
#include <iostream>

class DirectedGraph
{
public:
    DirectedGraph(size_t nr_nodes);

    int size() const;

    const std::vector<int>& neighbours(int node) const;
    const std::vector<int>& nodes() const;

    friend std::ostream& operator<<(std::ostream& out, const DirectedGraph& graph);

private:
    void generateRandomHamiltonian();

    std::vector<int> m_nodes;
    std::vector<std::vector<int>> m_neighbours;
};

