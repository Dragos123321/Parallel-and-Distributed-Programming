#pragma once

#include "graph.h"

#include <string>
#include <atomic>

int is_color_valid(int node, std::vector<int>& codes, Graph& graph);

int are_colors_valid(const std::vector<int>& codes, Graph& graph);

extern std::vector<std::string> colors;

std::map<int, std::string> get_nodes_to_colors(const std::vector<int>& codes);

std::map<int, std::string> graph_coloring_threads(int nr_threads, Graph& graph);

void graph_coloring_util_threads(int nr_threads, int node, std::vector<int> codes, std::vector<int>& res_codes, Graph& graph);

std::map<int, std::string> graph_coloring_master(int mpi_size, Graph& graph);
void graph_coloring_worker(int id, int mpi_size, int parent, Graph& graph);
void graph_coloring_final_worker(int mpi_size, int parent, Graph& graph);

int generate_next(std::vector<int>& codes, int start, int end, int limit, Graph& graph);