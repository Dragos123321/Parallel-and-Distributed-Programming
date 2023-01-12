#include "distributed_shared_memory.h"

#include "globals.h"
#include "mpi_utils.h"

void DSM::set_variable(const std::string& var, int new_value)
{
    variable(var).set_value(new_value);
}

Variable& DSM::variable(const std::string& var)
{
    if (var == "a") {
        return a;
    }
    else if (var == "b") {
        return b;
    }
    else {
        return c;
    }
}

void DSM::subscribe(const std::string& var)
{
    for (int id = 0; id < g_nr_procs; ++id) {
        send_subscribe_message(var, g_id, id);
    }
}

void DSM::update_subscription(const std::string& var, int id)
{
    variable(var).add_subscriber(id);
}

void DSM::update_variable(const std::string& var, int new_value)
{
    for (auto sub : variable(var).get_subscribers()) {
        send_update_message(var, new_value, sub);
    }
}

void DSM::check_and_replace(const std::string& var, int old_value, int new_value)
{
    Variable& v = variable(var);
    if (v.get_value() == old_value) {
        for (int id : v.get_subscribers())
            send_update_message(var, new_value, id);
    }
}

void DSM::close()
{
    for (int id = 0; id < g_nr_procs; ++id) {
        send_close_message(id);
    }
}
