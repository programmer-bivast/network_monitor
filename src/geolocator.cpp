#include "geolocator.h"
#include "utils.h"

#include <iostream>

std::string Geolocator::lookup(const std::string& ip) {
    auto it = cache_.find(ip);
    if (it != cache_.end()) return it->second;

    std::string response = utils::http_get("ip-api.com", "/json/" + ip);
    if (response.empty()) {
        cache_[ip] = "Unknown";
        return "Unknown";
    }

    std::string body = utils::strip_http_headers(response);
    std::string status = utils::extract_json_string(body, "status");

    std::string result;
    if (status == "success") {
        std::string country = utils::extract_json_string(body, "country");
        std::string city    = utils::extract_json_string(body, "city");
        result = city + ", " + country;
    } else {
        result = "Unknown";
    }

    cache_[ip] = result;
    return result;
}
