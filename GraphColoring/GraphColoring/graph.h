#pragma once

#include <vector>
#include <map>
#include <set>

class Graph
{
public:
    Graph(int nr_nodes);
    void addEdge(int node1, int node2);
    bool isEdge(int node1, int node2);

    const std::vector<int> nodes() const;
    const std::set<int> neighbours(int node);

private:
    std::vector<int> m_nodes;
    std::map<int, std::set<int>> m_neighbours;
};