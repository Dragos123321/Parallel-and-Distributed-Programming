#include "DirectedGraph.h"

#include <iostream>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <thread>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

std::atomic_bool found = false;
std::atomic_size_t nr_created_threads = 0;
std::mutex mx;
std::vector<int> result;

void hamilton_util(const DirectedGraph& graph, int node, int starting_node, std::vector<int>& path);
void hamilton_util_parallel(const DirectedGraph& graph, int node, int starting_node, std::vector<int> path,
                            boost::asio::thread_pool& pool);

void hamilton(const DirectedGraph& graph, int starting_node) {
    std::vector<int> path;

    boost::asio::thread_pool pool(std::thread::hardware_concurrency());

    hamilton_util_parallel(graph, starting_node, starting_node, path, pool);

    pool.join();
}

void hamilton_util(const DirectedGraph& graph, int node, int starting_node, std::vector<int>& path) {
    path.push_back(node);

    if (!found) {
        if (path.size() == graph.size()) {
            if (std::find(graph.neighbours(node).begin(), graph.neighbours(node).end(), starting_node) != graph.neighbours(node).end()) {
                if (!found.exchange(true)) {
                    std::lock_guard<std::mutex> lk(mx);
                    result.clear();
                    result.insert(result.end(), path.begin(), path.end());
                }
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

    path.pop_back();
}

void hamilton_util_parallel(const DirectedGraph& graph, int node, int starting_node, std::vector<int> path,
                            boost::asio::thread_pool& pool) {
    path.push_back(node);

    if (!found) {
        if (path.size() == graph.size()) {
            if (std::find(graph.neighbours(node).begin(), graph.neighbours(node).end(), starting_node) != graph.neighbours(node).end()) {
                if (!found.exchange(true)) {
                    std::lock_guard<std::mutex> lk(mx);
                    result.clear();
                    result.insert(result.end(), path.begin(), path.end());
                }
            }
        }
        else {
            for (auto neigh : graph.neighbours(node)) {
                if (std::find(path.begin(), path.end(), neigh) == path.end()) {
                    if (nr_created_threads.fetch_add(1) > std::thread::hardware_concurrency() ) {
                        nr_created_threads.fetch_sub(1);
                        hamilton_util(graph, neigh, starting_node, path);
                    }
                    else {
                        boost::asio::post(pool, [&graph, neigh, starting_node, path, &pool] () {
                                hamilton_util_parallel(graph, neigh, starting_node, path, pool);
                            });
                    }
                }
            }
        }
    }
}

void run(int nr_nodes, std::ofstream& out) {
    if (nr_nodes == 0) {
        return;
    }

    DirectedGraph graph(nr_nodes);

    auto start = std::chrono::high_resolution_clock::now();

    hamilton(graph, 0);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    out << "|" << std::setw(10) << nr_nodes << " | " << std::setw(10) << static_cast<int>(duration.count()) << " |\n";
}

int main() {
    std::ofstream out("D:\\Dragos\\Work\\Faculty\\Programare paralela\\lab6_clion\\stats.txt");

    out << "+-----------+------------+" << '\n';
    out << "|  Nr nodes |   Time(ms) |\n";
    out << "+-----------+------------+" << '\n';

    for (int i = 0; i <= 10000; i += 500) {
        found = false;
        nr_created_threads = 0;
        run(i, out);
    }

    out << "+-----------+------------+\n";

    out.close();
}