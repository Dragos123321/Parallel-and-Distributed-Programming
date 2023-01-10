#pragma once

extern int id;
extern int parent;
extern int procs;
extern int algorithm_type; // 0 - karatsuba, 1 - naive

void set_algorithm_type(int new_algorithm_type);
void set_id(int new_id);
void set_parent(int new_parent);
void set_procs(int new_procs);