#pragma once

#include "variable.h"

class DSM
{
public:
    void set_variable(const std::string& var, int new_value);
    Variable& variable(const std::string& var);

    void subscribe(const std::string& var);
    void update_subscription(const std::string& var, int id);

    void update_variable(const std::string& var, int new_value);
    void check_and_replace(const std::string& var, int old_value, int new_value);

    static void close();

private:
    Variable a;
    Variable b;
    Variable c;
};

