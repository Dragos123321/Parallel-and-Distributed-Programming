#define _CRT_SECURE_NO_WARNINGS
#include "graph_coloring.h"

#include <stdexcept>
#include <mpi.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

extern std::vector<std::string> colors = std::vector<std::string>();

std::mutex mx;

int is_color_valid(int node, const std::vector<int>& codes, Graph& graph) {
    for (auto n_node : graph.neighbours(node)) {
        if (n_node < node && codes[n_node] == codes[node] && codes[node] != -1 && codes[n_node] != -1) {
            return 0;
        }
    }

    return 1;
}

int are_colors_valid(const std::vector<int>& codes, Graph& graph)
{
    for (auto node : graph.nodes()) {
        if (!is_color_valid(node, codes, graph)) {
            return 0;
        }
    }

    return 1;
}

std::map<int, std::string> get_nodes_to_colors(const std::vector<int>& codes)
{
    std::map<int, std::string> res;

    for (size_t i = 0; i < codes.size(); ++i) {
        res[i] = colors[codes[i]];
    }

    return res;
}

std::map<int, std::string> graph_coloring_threads(int nr_threads, Graph& graph)
{
    std::vector<int> codes = std::vector<int>(graph.nodes().size(), 0);
    std::vector<int> res_codes;

    std::atomic_int nr_threads_atomic = nr_threads;

    graph_coloring_util_threads(nr_threads_atomic, 0, codes, res_codes, graph);

    if (res_codes.empty())
        throw std::runtime_error("No solution found!");

    return get_nodes_to_colors(res_codes);
}

void graph_coloring_util_threads(std::atomic_int& nr_threads, int node, std::vector<int> codes, std::vector<int>& res_codes, Graph& graph)
{
    if (!res_codes.empty())
        return;

    if (node + 1 == graph.nodes().size()) {
        if (is_color_valid(node, codes, graph)) {
            std::lock_guard<std::mutex> lk(mx);

            if (res_codes.empty()) {
                res_codes.insert(res_codes.begin(), codes.begin(), codes.end());
            }
        }

        return;
    }

    int next_node = node + 1;

    std::vector<std::thread> threads;
    std::vector<int> valid_codes;

    for (int code = 0; code < colors.size(); ++code) {
        codes[next_node] = code;

        if (is_color_valid(next_node, codes, graph)) {

            if (nr_threads.fetch_sub(1) > 0) {
                auto next_codes = codes;

                threads.emplace_back([&nr_threads, next_node, next_codes, &res_codes, &graph] { graph_coloring_util_threads(nr_threads, next_node, next_codes, res_codes, graph); });
            }
            else {
                valid_codes.push_back(code);
            }
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int code : valid_codes) {
        codes[next_node] = code;
        auto next_codes = codes;

        graph_coloring_util_threads(nr_threads, next_node, next_codes, res_codes, graph);;
    }
}

std::map<int, std::string> graph_coloring_master(int mpi_size, Graph& graph)
{
    auto nr_nodes_per_node = graph.nodes().size() / mpi_size;

    if (mpi_size == 1) {
        return graph_coloring_threads(std::thread::hardware_concurrency(), graph);
    }

    auto start = 0;
    auto end = nr_nodes_per_node;
    std::vector<int> codes(graph.nodes().size(), -1);

    std::vector<int> res;

    while (true) {
        int can_gen_1 = generate_next(codes, start, end, nr_nodes_per_node * mpi_size, graph);
        int can_gen_2 = generate_next(codes, nr_nodes_per_node * mpi_size, graph.nodes().size(), nr_nodes_per_node * mpi_size, graph);

        while (can_gen_1 == 1 && can_gen_2 == 0) {
            for (int i = nr_nodes_per_node * mpi_size; i < graph.nodes().size(); ++i) {
                codes[i] = -1;
            }
            can_gen_1 = generate_next(codes, start, end, nr_nodes_per_node * mpi_size, graph);
            can_gen_2 = generate_next(codes, nr_nodes_per_node * mpi_size, graph.nodes().size(), nr_nodes_per_node * mpi_size, graph);
        }

        int can_gen = can_gen_1 & can_gen_2;

        MPI_Ssend(&can_gen, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Ssend(codes.data(), codes.size(), MPI_INT, 1, 1, MPI_COMM_WORLD);

        if (can_gen == false) {
            throw std::runtime_error("No solution found.");
        }

        int found;
        MPI_Recv(&found, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(codes.data(), codes.size(), MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (found) {
            return get_nodes_to_colors(codes);
        }
    }
}

void graph_coloring_worker(int id, int mpi_size, int parent, Graph& graph)
{
    auto nr_nodes_per_node = graph.nodes().size() / mpi_size;

    auto start = id * nr_nodes_per_node;
    auto end = (id + 1) * nr_nodes_per_node;

    while (true) {
        int could_generate;
        MPI_Recv(&could_generate, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::vector<int> codes(graph.nodes().size(), 0);
        MPI_Recv(codes.data(), codes.size(), MPI_INT, parent, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (!could_generate) {
            MPI_Ssend(&could_generate, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);
            MPI_Ssend(codes.data(), codes.size(), MPI_INT, id + 1, 1, MPI_COMM_WORLD);
            break;
        }

        _NotFound:

        int can_gen = generate_next(codes, start, end, nr_nodes_per_node * mpi_size, graph);

        if (!can_gen) {
            MPI_Ssend(&can_gen, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
            MPI_Ssend(codes.data(), codes.size(), MPI_INT, parent, 1, MPI_COMM_WORLD);
        }
        else {
            MPI_Ssend(&can_gen, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);
            MPI_Ssend(codes.data(), codes.size(), MPI_INT, id + 1, 1, MPI_COMM_WORLD);

            int found;
            MPI_Recv(&found, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(codes.data(), codes.size(), MPI_INT, id + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (found) {
                MPI_Ssend(&found, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
                MPI_Ssend(codes.data(), codes.size(), MPI_INT, parent, 1, MPI_COMM_WORLD);
                break;
            }
            else {
                goto _NotFound;
            }
        }
    }
}

void graph_coloring_final_worker(int mpi_size, int parent, Graph& graph)
{
    int id = mpi_size - 1;
    auto nr_nodes_per_node = graph.nodes().size() / mpi_size;

    auto start = id * nr_nodes_per_node;
    auto end = (id + 1) * nr_nodes_per_node;

    while (true) {
        int could_generate;
        MPI_Recv(&could_generate, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::vector<int> codes(graph.nodes().size(), 0);
        MPI_Recv(codes.data(), codes.size(), MPI_INT, parent, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (!could_generate) {
            break;
        }

        int can_gen = generate_next(codes, start, end, nr_nodes_per_node * mpi_size, graph);

        if (can_gen) {
            MPI_Ssend(&can_gen, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
            MPI_Ssend(codes.data(), codes.size(), MPI_INT, parent, 1, MPI_COMM_WORLD);
            break;
        }
        else {
            MPI_Ssend(&can_gen, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
            MPI_Ssend(codes.data(), codes.size(), MPI_INT, parent, 1, MPI_COMM_WORLD);
        }
    }
}

int gen_next_util(std::vector<int>& codes, int start, int end, int limit) {
    for (int i = end - 1; i >= start; --i) {
        if (codes[i] != colors.size() - 1) {
            codes[i] += 1;
            for (int j = i + 1; j < limit; ++j) {
                codes[j] = j >= end ? -1 : 0;
            }
            return 1;
        }
    }

    return 0;
}

int generate_next(std::vector<int>& codes, int start, int end, int limit, Graph& graph)
{
    if (start == end) {
        return 1;
    }

    if (codes[start] == -1) {
        for (int i = start; i < end; ++i) {
            codes[i] = 0;
        }
        if (are_colors_valid(codes, graph)) {
            return 1;
        }
    }

    int found = gen_next_util(codes, start, end, limit);
    if (!found) {
        return 0;
    }

    if (found && are_colors_valid(codes, graph)) {
        return 1;
    }
    while (!are_colors_valid(codes, graph)) {
        found = gen_next_util(codes, start, end, limit);

        if (!found) {
            return 0;
        }

        if (are_colors_valid(codes, graph)) {
            return 1;
        }
    }
}
