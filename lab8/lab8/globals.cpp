#include "globals.h"

int g_id;
int g_nr_procs;

void set_id(int new_id)
{
    g_id = new_id;
}

void set_nr_procs(int new_nr_procs)
{
    g_nr_procs = new_nr_procs;
}