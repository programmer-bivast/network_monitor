#include "utils.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

std::string utils::http_get(const std::string& host, const std::string& path) {
    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res = nullptr;
    int gai_err = getaddrinfo(host.c_str(), "80", &hints, &res);
    if (gai_err != 0 || !res) {
        std::cerr << "getaddrinfo: " << gai_strerror(gai_err) << std::endl;
        return {};
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        std::cerr << "socket: " << std::strerror(errno) << std::endl;
        freeaddrinfo(res);
        return {};
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "connect: " << std::strerror(errno) << std::endl;
        close(sock);
        freeaddrinfo(res);
        return {};
    }
    freeaddrinfo(res);

    std::string request = "GET " + path + " HTTP/1.1\r\n"
                          "Host: " + host + "\r\n"
                          "Connection: close\r\n"
                          "\r\n";

    ssize_t sent = send(sock, request.data(), request.size(), 0);
    if (sent < 0) {
        std::cerr << "send: " << std::strerror(errno) << std::endl;
        close(sock);
        return {};
    }

    std::string response;
    char buf[4096];
    ssize_t n;
    while ((n = recv(sock, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[n] = '\0';
        response.append(buf, static_cast<size_t>(n));
    }

    close(sock);
    return response;
}

std::string utils::strip_http_headers(const std::string& response) {
    auto pos = response.find("\r\n\r\n");
    if (pos == std::string::npos) return response;
    return response.substr(pos + 4);
}

std::string utils::extract_json_string(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":\"";
    auto pos = json.find(search);
    if (pos == std::string::npos) return {};
    pos += search.size();
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            result += json[pos + 1];
            pos += 2;
        } else {
            result += json[pos];
            ++pos;
        }
    }
    return result;
}

std::string utils::ip_to_string(uint32_t ip) {
    struct in_addr addr{};
    addr.s_addr = ip;
    return inet_ntoa(addr);
}
