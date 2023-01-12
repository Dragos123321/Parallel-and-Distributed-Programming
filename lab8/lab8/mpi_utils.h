#pragma once

#include <vector>
#include <string>

void send_update_message(const std::string& var, int new_value, int id);
void send_subscribe_message(const std::string& var, int new_id, int id);
void send_close_message(int id);
std::vector<int> get_message();

