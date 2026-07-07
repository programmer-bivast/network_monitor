#include "sniffer.h"
#include "geolocator.h"
#include "utils.h"

#include <cstring>
#include <iostream>

#include <net/ethernet.h>
#include <netinet/ip.h>

Sniffer::Sniffer(const std::string& interface, uint32_t our_ip)
    : handle_(nullptr), interface_(interface), our_ip_(our_ip) {}

Sniffer::~Sniffer() {
    stop();
}

void Sniffer::stop() {
    running_ = false;
    if (handle_) {
        pcap_breakloop(handle_);
    }
}

void Sniffer::start_capture() {
    char errbuf[PCAP_ERRBUF_SIZE];
    handle_ = pcap_open_live(interface_.c_str(), BUFSIZ, 0, 1000, errbuf);
    if (!handle_) {
        std::cerr << "pcap_open_live: " << errbuf << "\n";
        return;
    }

    struct bpf_program fp{};
    if (pcap_compile(handle_, &fp, "ip", 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "pcap_compile: " << pcap_geterr(handle_) << "\n";
        return;
    }
    if (pcap_setfilter(handle_, &fp) == -1) {
        std::cerr << "pcap_setfilter: " << pcap_geterr(handle_) << "\n";
        pcap_freecode(&fp);
        return;
    }
    pcap_freecode(&fp);

    running_ = true;
    std::cout << "Listening on " << interface_ << " ...\n";
    pcap_loop(handle_, -1, packet_handler, reinterpret_cast<unsigned char*>(this));
}

void Sniffer::packet_handler(unsigned char* user, const struct pcap_pkthdr* hdr, const unsigned char* packet) {
    (void)hdr;
    auto* sniffer = reinterpret_cast<Sniffer*>(user);
    if (!sniffer->running_) return;

    if (hdr->len < sizeof(struct ether_header) + sizeof(struct ip)) return;

    auto* ip_hdr = reinterpret_cast<const struct ip*>(packet + sizeof(struct ether_header));

    if (ip_hdr->ip_src.s_addr != sniffer->our_ip_) return;

    std::string dest_ip = utils::ip_to_string(ip_hdr->ip_dst.s_addr);

    static Geolocator geo;
    std::string location = geo.lookup(dest_ip);

    std::cout << "-> " << dest_ip << "  (" << location << ")\n";
}
