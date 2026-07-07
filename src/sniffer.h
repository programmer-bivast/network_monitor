#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include <pcap.h>

class Sniffer {
public:
    Sniffer(const std::string& interface, uint32_t our_ip);
    ~Sniffer();

    void start_capture();
    void stop();

private:
    static void packet_handler(unsigned char* user, const struct pcap_pkthdr* header, const unsigned char* packet);

    pcap_t* handle_;
    std::string interface_;
    uint32_t our_ip_;
    std::atomic<bool> running_{false};
};
