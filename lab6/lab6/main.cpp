#include "DirectedGraph.h"

#include <iostream>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <fstream>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

std::atomic_bool found = false;
std::mutex mx;
std::vector<int> result;
int cnt;

void hamilton_util(const DirectedGraph& graph, int node, int starting_node, std::vector<int>& path);

void hamilton(const DirectedGraph& graph, int starting_node) {
    std::vector<int> path;
    hamilton_util(graph, starting_node, starting_node, path);
}

void hamilton_util(const DirectedGraph& graph, int node, int starting_node, std::vector<int>& path) {
    path.push_back(node);

    if (!found) {
        if (path.size() == graph.size()) {
            if (std::find(graph.neighbours(node).begin(), graph.neighbours(node).end(), starting_node) != graph.neighbours(node).end()) {
                found = true;
                std::lock_guard<std::mutex> lk(mx);
                result.clear();
                result.insert(result.end(), path.begin(), path.end());
            }
        }
        else {
            for (auto neigh : graph.neighbours(node)) {
                if (std::find(path.begin(), path.end(), neigh) == path.end()) {
                    hamilton_util(graph, neigh, starting_node, path);
                }
            }
        }
    }
}

void run(int nr_nodes, std::ofstream& out) {
    if (nr_nodes == 0) {
        return;
    }

    boost::asio::thread_pool pool(std::thread::hardware_concurrency());

    DirectedGraph graph(nr_nodes);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < graph.size(); ++i) {
        boost::asio::post(pool, [&graph, i]() { hamilton(graph, i); });
    }

    pool.join();

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    out << "|" << std::setw(10) << nr_nodes << " | " << std::setw(10) << static_cast<int>(duration.count()) << " |\n";
}

int main() {
    std::ofstream out("stats.txt");

    out << "+-----------+------------+" << '\n';
    out << "|  Nr nodes |   Time(ms) |\n";
    out << "+-----------+------------+" << '\n';

    for (size_t i = 0; i <= 100000; i += 500) {
        run(i, out);
    }

    out << "+-----------+------------+\n";

    out.close();
}