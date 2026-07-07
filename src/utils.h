#pragma once

#include <string>
#include <cstdint>

namespace utils {

std::string http_get(const std::string& host, const std::string& path);
std::string strip_http_headers(const std::string& response);
std::string extract_json_string(const std::string& json, const std::string& key);
std::string ip_to_string(uint32_t ip);

} // namespace utils
