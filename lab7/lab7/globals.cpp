#include "globals.h"

int id;
int parent;
int procs;
int algorithm_type; // 0 - karatsuba, 1 - naive

void set_id(int new_id)
{
    id = new_id;
}

void set_parent(int new_parent)
{
    parent = new_parent;
}

void set_procs(int new_procs)
{
    procs = new_procs;
}

void set_algorithm_type(int new_algorithm_type)
{
    algorithm_type = algorithm_type;
}