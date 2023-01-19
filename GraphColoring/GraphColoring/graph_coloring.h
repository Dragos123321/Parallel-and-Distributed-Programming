#pragma once

#include "graph.h"

#include <string>
#include <atomic>

/** @brief Checks if the coloring of the current node is correct respective to it's neighbours.
 *
 *  @param node  The node to check.
 *  @param codes A vector representing the coloring of all nodes in the graph.
 *  @param graph The graph the node is part of.
 *
 *  @return 1 if the coloring of the node is correct, false otherwise.
 *
**/
int is_color_valid(int node, const std::vector<int>& codes, Graph& graph);

/** @brief Checks if the coloring for a graph is correct.
 *
 *  @param codes A vector representing the coloring of all nodes in the graph.
 *  @param graph The graph to check.
 *
 *  @return 1 if the coloring of the graph is correct, false otherwise.
 *
**/
int are_colors_valid(const std::vector<int>& codes, Graph& graph);

extern std::vector<std::string> colors;

/** @brief Translates the color's code of each node to it's respective string.
 *
 *  @param codes A vector representing the coloring of all nodes in the graph.
 *
 *  @return A map representing the relation between a color's code and the color.
 *
**/
std::map<int, std::string> get_nodes_to_colors(const std::vector<int>& codes);

/** @brief Creates the context for running the parallel algorithm for graph coloring and calls \a graph_coloring_threads.
 *
 *  @param nr_threads The number of threads to run the algorithm on.
 *  @param graph      The graph to color.
 *
 *  @return A map representing the relation between a color's code and the color created using \a get_nodes_to_colors.
 *
**/
std::map<int, std::string> graph_coloring_threads(int nr_threads, Graph& graph);

/** @brief Tries to find a valid coloring for a graph. 
 *  If the \p node is the last node and the algorithm was able to generate a valid coloring it will lock the global mutex and 
 *  add the resulting coloring to res_codes.
 *  Otherwise the algorithm will set the coloring of the next_node to the next possible color and will try to start a new thread to continue the
 *  coloring starting with this node and it's respective color.
 *  After all the threads will be joined and if no valid coloring is found the funtion will try to run the algorithm sequentially using the other potential colors
 *  for the current node(the ones that could not be run on a thread).
 *
 *  @param nr_threads The number of threads to run the algorithm on.
 *  @param node       The current node.
 *  &param codes      The codes representing the color for each node.
 *  @param graph      The graph to color.
 *
 *  @return A map representing the relation between a color's code and the color created using \a get_nodes_to_colors.
 *
**/
void graph_coloring_util_threads(std::atomic_int& nr_threads, int node, std::vector<int> codes, std::vector<int>& res_codes, Graph& graph);

/** @brief Tries to find a valid coloring for a graph.
 *  Generates possible valid colorings for the first section of nodes in the graph and for the last one if it exists and is not covered by the last worker.
 *  It sends the colorings and if it was able to generate a new coloring to it's child and waits for it's result. This process happens will run until
 *  a coloring for the whole graph is found or it is not able to generate a new coloring.
 *
 *  @param mpi_size The number of nodes.
 *  @param graph      The graph to color.
 *
 *  @return A map representing the relation between a color's code and the color created using \a get_nodes_to_colors.
 *
**/
std::map<int, std::string> graph_coloring_master(int mpi_size, Graph& graph);

/** @brief Tries to find a valid coloring for a graph.
 *  Generates possible valid colorings for it's section of nodes in the graph. 
 *  It will comunicate with it's parent and child in order to find a solution.
 *
 *  @param mpi_size The number of nodes.
 *  @param graph      The graph to color.
 *
**/
void graph_coloring_worker(int id, int mpi_size, int parent, Graph& graph);

/** @brief Tries to find a valid coloring for a graph.
 *  Generates possible valid colorings for it's section of nodes in the graph.
 *  It will comunicate with it's parent in order to find a solution.
 *
 *  @param mpi_size The number of nodes.
 *  @param graph      The graph to color.
 *
**/
void graph_coloring_final_worker(int mpi_size, int parent, Graph& graph);

/** @brief Generates the next possible coloring of the nodes in range \p start - \p end.
 *
 *  @param codes A vector representing the coloring of all nodes in the graph.
 *  @param start The start of the sequence to change.
 *  @param codes The end of the sequence to change.
 *  @param limit The last element the last child will edit + 1
 *  @param graph The graph to color.
 *
 *  @return 1 if a new color generation is possible, 0 otherwise.
 *
**/
int generate_next(std::vector<int>& codes, int start, int end, int limit, Graph& graph);