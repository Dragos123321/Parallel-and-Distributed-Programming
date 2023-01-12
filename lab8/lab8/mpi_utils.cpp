#include "mpi_utils.h"

#include <mpi.h>
#include <sstream>
#include <iostream>

void send_update_message(const std::string& var, int new_value, int id)
{
    int nr_var = 0;
    if (var == "a")
        nr_var = 0;
    else if (var == "b")
        nr_var = 1;
    else if (var == "c")
        nr_var = 2;

    std::vector<int> arr = { 0, nr_var, new_value };

    std::stringstream ss;
    ss << "Sending " << arr[0] << " " << arr[1] << " " << arr[2] << " to " << id;

    std::cout << ss.str() << '\n';

    MPI_Bsend(arr.data(), 3, MPI_INT, id, 0, MPI_COMM_WORLD);
}

void send_subscribe_message(const std::string& var, int new_id, int id)
{
    int nr_var = 0;
    if (var == "a")
        nr_var = 0;
    else if (var == "b")
        nr_var = 1;
    else if (var == "c")
        nr_var = 2;

    std::vector<int> arr = { 1, nr_var, new_id };

    std::stringstream ss;
    ss << "Sending " << arr[0] << " " << arr[1] << " " << arr[2] << " to " << id;

    std::cout << ss.str() << '\n';

    MPI_Bsend(arr.data(), 3, MPI_INT, id, 0, MPI_COMM_WORLD);
}

void send_close_message(int id)
{
    std::vector<int> arr = { 2, 0, 0 };

    std::stringstream ss;

    ss << "Sending " << arr[0] << " " << arr[1] << " " << arr[2] << " to " << id;
    std::cout << ss.str() << '\n';

    MPI_Bsend(arr.data(), 3, MPI_INT, id, 0, MPI_COMM_WORLD);
}

std::vector<int> get_message()
{
    std::vector<int> arr(3, 0);

    MPI_Recv(arr.data(), 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::cout << "Recv: " << arr[0] << " " << arr[1] << " " << arr[2] << '\n';

    return arr;
}