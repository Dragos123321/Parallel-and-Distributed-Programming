#include "distributed_shared_memory.h"
#include "mpi_utils.h"
#include "globals.h"

#include <iostream>
#include <mpi.h>
#include <chrono>

using namespace std::chrono_literals;

std::mutex mx;
DSM dsm;
std::vector<int> attached(100000, 0);

void listener() {
    while (true) {
        auto message = get_message();

        std::string var;

        if (message[1] == 0) {
            var = "a";
        }
        else if (message[1] == 1) {
            var = "b";
        }
        else {
            var = "c";
        }

        std::cout << "Id: " << g_id << " a: " << dsm.variable("a").get_value() << " b: " << dsm.variable("b").get_value() << " c: " << dsm.variable("c").get_value() << '\n';

        if (message[0] == 0) {
            std::cout << g_id << ": Updating " << var << " with " << message[2] << '\n';
            mx.lock();
            dsm.set_variable(var, message[2]);
            mx.unlock();
        }
        else if (message[0] == 1) {
            std::cout << g_id << ": Subscribing " << var << " from " << message[2] << '\n';
            mx.lock();
            dsm.update_subscription(var, message[2]);
            mx.unlock();
        }
        else if (message[0] == 2) {
            std::cout << g_id << ": Quitting...\n";
            break;
        }
    }
}

int main() {
    MPI_Init(0, 0);
    int id, nr_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    MPI_Buffer_attach(attached.data(), attached.size());

    set_id(id);
    set_nr_procs(nr_procs);

    std::thread th([]() { listener(); });

    if (g_id == 0) {
        mx.lock();
        dsm.subscribe("a");
        mx.unlock();

        mx.lock();
        dsm.subscribe("b");
        mx.unlock();

        mx.lock();
        dsm.subscribe("c");
        mx.unlock();

        std::this_thread::sleep_for(1000ms);

        mx.lock();
        dsm.update_variable("a", 2);
        mx.unlock();

        mx.lock();
        dsm.update_variable("c", 4);
        mx.unlock();

        std::this_thread::sleep_for(2000ms);

        mx.lock();
        dsm.check_and_replace("c", 4, 6);
        mx.unlock();

        DSM::close();
    }
    else if (g_id == 1) {
        mx.lock();
        dsm.subscribe("a");
        mx.unlock();

        mx.lock();
        dsm.subscribe("b");
        mx.unlock();

        std::this_thread::sleep_for(1000ms);

        mx.lock();
        dsm.update_variable("a", 6);
        mx.unlock();

        mx.lock();
        dsm.check_and_replace("b", 0, 3);
        mx.unlock();
    }
    else if (g_id == 2) {
        mx.lock();
        dsm.subscribe("c");
        mx.unlock();

        std::this_thread::sleep_for(1000ms);
        
        mx.lock();
        std::cout << dsm.variable("c").get_value() << '\n';
        mx.unlock();
    }

    th.join();

    MPI_Finalize();
}