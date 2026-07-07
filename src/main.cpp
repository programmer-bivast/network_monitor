#include "sniffer.h"

#include <csignal>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/types.h>

static Sniffer* g_sniffer = nullptr;

static void handle_signal(int) {
    if (g_sniffer) {
        std::cerr << "\nShutting down ...\n";
        g_sniffer->stop();
    }
}

static uint32_t get_interface_ip(const std::string& iface) {
    struct ifaddrs* ifaddr = nullptr;
    if (getifaddrs(&ifaddr) == -1) {
        std::cerr << "getifaddrs: " << std::strerror(errno) << "\n";
        return 0;
    }

    uint32_t ip = 0;
    for (auto* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
        if (iface != ifa->ifa_name) continue;
        ip = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr)->sin_addr.s_addr;
        break;
    }
    freeifaddrs(ifaddr);
    return ip;
}

int main(int argc, char* argv[]) {
    std::string interface = "en0";
    if (argc > 1) {
        interface = argv[1];
    }

    uint32_t our_ip = get_interface_ip(interface);
    if (our_ip == 0) {
        std::cerr << "error: could not determine IP for interface '" << interface << "'\n";
        return 1;
    }

    struct in_addr addr{};
    addr.s_addr = our_ip;
    std::cout << "Monitoring outbound traffic on " << interface
              << " (" << inet_ntoa(addr) << ")\n"
              << "Press Ctrl+C to stop.\n";

    std::signal(SIGINT, handle_signal);

    Sniffer sniffer(interface, our_ip);
    g_sniffer = &sniffer;
    sniffer.start_capture();

    return 0;
}
