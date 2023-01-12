#include "variable.h"

Variable::Variable()
{
}

Variable::~Variable()
{
}

void Variable::set_value(int new_value)
{
    m_value = new_value;
}

void Variable::add_subscriber(int id)
{
    m_subscribers.push_back(id);
}

int Variable::get_value() const
{
    return m_value;
}

const std::vector<int>& Variable::get_subscribers() const
{
    return m_subscribers;
}
