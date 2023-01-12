#pragma once

#include <vector>
#include <mutex>

class Variable {
public:
    Variable();
    ~Variable();

    void set_value(int new_value);
    void add_subscriber(int id);

    int get_value() const;
    const std::vector<int>& get_subscribers() const;
private:
    int m_value;
    std::vector<int> m_subscribers;
};
