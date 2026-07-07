#pragma once

#include <string>
#include <unordered_map>

class Geolocator {
public:
    std::string lookup(const std::string& ip);

private:
    std::unordered_map<std::string, std::string> cache_;
};
