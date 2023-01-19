#include "graph_coloring.h"

#include <stdexcept>
#include <mpi.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    MPI_Init(0, 0);

    int id;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    colors.push_back("red");
    colors.push_back("green");
    colors.push_back("blue");
    colors.push_back("yellow");
    colors.push_back("orange");

    Graph graph(100);

   /* graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    graph.addEdge(4, 0);
    graph.addEdge(2, 0);
    graph.addEdge(3, 1);
    graph.addEdge(1, 4);
    graph.addEdge(5, 7);
    graph.addEdge(5, 3);
    graph.addEdge(8, 9);
    graph.addEdge(0, 6);
    graph.addEdge(7, 9);
    graph.addEdge(2, 9);*/

    if (id == 0) {
        try {
            auto start = std::chrono::high_resolution_clock::now();

            auto res = graph_coloring_master(size, graph);

            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> duration = end - start;

            std::cout << "The algorithm took: " << duration.count() << "ms\n";

            for (auto el : res) {
                std::cout << el.first << ": " << el.second << '\n';
            }
        }
        catch (const std::runtime_error& err) {
            std::cout << err.what() << '\n';
        }
    }
    else if (id == size - 1) {
        graph_coloring_final_worker(size, id - 1, graph);
    }
    else {
        graph_coloring_worker(id, size, id - 1, graph);
    }

    MPI_Finalize();

    return 0;
}