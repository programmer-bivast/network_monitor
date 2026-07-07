# Network Monitor

A C++17 tool that monitors outbound network traffic on a given interface using libpcap and geolocates destination IPs.

## Prerequisites

- C++17 compiler (Clang recommended)
- [libpcap](https://www.tcpdump.org/) (development headers)
- CMake or Make

## Building

```sh
make
```

## Usage

```sh
sudo ./netmon [interface]
```

Defaults to `en0`. Requires root for packet capture.

## License

MIT
