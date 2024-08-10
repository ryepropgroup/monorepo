#include "mach/broadcast.hpp"
#include <iostream>
#include <cstring>
#include <spdlog/spdlog.h>
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <iphlpapi.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <ifaddrs.h>
    #include <unistd.h>
#endif

#ifdef _WIN32
static std::string getBroadcastAddress() {
    std::unique_ptr<IP_ADAPTER_ADDRESSES[]> adapter_addresses;
    ULONG size = 0;
    DWORD result = GetAdaptersAddresses(AF_INET, 0, nullptr, nullptr, &size);

    if (result == ERROR_BUFFER_OVERFLOW) {
        adapter_addresses = std::make_unique<IP_ADAPTER_ADDRESSES[]>(size);
        result = GetAdaptersAddresses(AF_INET, 0, nullptr, adapter_addresses.get(), &size);
    }

    if (result != ERROR_SUCCESS) {
        spdlog::error("MACH: GetAdaptersAddresses failed with error: {}", result);
        return "";
    }

    std::string broadcast_address;
    IP_ADAPTER_ADDRESSES* adapter = adapter_addresses.get();

    // Find an adapter where the assigned ip is 192.168.x.x.
    while (adapter) {
        IP_ADAPTER_UNICAST_ADDRESS* unicast_address = adapter->FirstUnicastAddress;
        while (unicast_address) {
            sockaddr_in* addr = (sockaddr_in*)unicast_address->Address.lpSockaddr;
            if (!addr || addr->sin_family != AF_INET || addr->sin_addr.S_un.S_un_b.s_b1 != 192 || addr->sin_addr.S_un.S_un_b.s_b2 != 168) {
                unicast_address = unicast_address->Next;
                continue;
            }

            char ip_str[INET_ADDRSTRLEN];
            char mask_str[INET_ADDRSTRLEN];

            // Convert IP address to string.
            inet_ntop(AF_INET, &addr->sin_addr, ip_str, INET_ADDRSTRLEN);
            
            // Get the subnet mask.
            DWORD prefixLength = unicast_address->OnLinkPrefixLength;
            DWORD maskLong = 0xFFFFFFFF << (32 - prefixLength);
            sockaddr_in mask_addr = { 0 };
            mask_addr.sin_addr.s_addr = htonl(maskLong);
            inet_ntop(AF_INET, &mask_addr.sin_addr, mask_str, INET_ADDRSTRLEN);
            
            // Convert to in_addr.
            in_addr ip, mask, broadcast;
            inet_pton(AF_INET, ip_str, &ip);
            inet_pton(AF_INET, mask_str, &mask);

            // Calculate broadcast address
            broadcast.s_addr = (ip.s_addr & mask.s_addr) | (~mask.s_addr);

            char broadcast_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &broadcast, broadcast_ip, INET_ADDRSTRLEN);

            broadcast_address = std::string(broadcast_ip);
            break;
        }
        if (!broadcast_address.empty()) {
            break;
        }
        adapter = adapter->Next;
    }

    return broadcast_address;
}
#else
static std::string getBroadcastAddress() {
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        spdlog::error("MACH: Failed to get network interfaces!");
        return "";
    }

    std::string broadcast_address = "";

    // Find an adapter where the assigned ip is 192.168.x.x.
    for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
        // int first = addr->sin_addr.s_addr >> 24;
        // int second = addr->sin_addr.s_addr >> 16 & 0xFF;
        // int third = addr->sin_addr.s_addr >> 8 & 0xFF;
        // int fourth = addr->sin_addr.s_addr & 0xFF;
        // spdlog::info("Found: {}.{}.{}.{}", first, second, third, fourth);

        if (addr->sin_addr.s_addr & 0xFF != 192 || (addr->sin_addr.s_addr >> 8 & 0xFF) != 168) {
            continue;
        }

        struct sockaddr_in* netmask = (struct sockaddr_in*)ifa->ifa_netmask;
        struct sockaddr_in broadcast;
        broadcast.sin_family = AF_INET;
        // Calculate broadcast address.
        broadcast.sin_addr.s_addr = (addr->sin_addr.s_addr & netmask->sin_addr.s_addr) | (~netmask->sin_addr.s_addr);

        char broadcast_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &broadcast.sin_addr, broadcast_ip, INET_ADDRSTRLEN);

        broadcast_address = std::string(broadcast_ip);
        break;
    }

    freeifaddrs(ifaddr);
    return broadcast_address;
}
#endif


static int initSocket() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        spdlog::error("MACH: Broadcast WSAStartup failed.");
        return -1;
    }
#endif
    return 0;
}

static void cleanupSocket() {
#ifdef _WIN32
    WSACleanup();
#endif
}


static int createBroadcastSocket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        spdlog::error("MACH: Broadcast socket creation failed.");
        return -1;
    }

    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastEnable, sizeof(broadcastEnable)) < 0) {
        spdlog::error("MACH: Error in setting broadcast options.");
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return -1;
    }

    return sock;
}

static struct sockaddr_in setupBroadcastAddress(std::string broadcastIp) {
    struct sockaddr_in broadcastAddr = {};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(6969);
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIp.c_str());

    return broadcastAddr;
}

static void sendBroadcastMessage(int sock, const struct sockaddr_in& broadcastAddr) {
    std::string message("MACH Engine Computer");
    if (sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        spdlog::error("MACH: Broadcast send failed.");
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return;
    }

    spdlog::info("MACH: Broadcast message sent.");
}

static void closeSocket(int sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

static int broadcastServer() {
    if (initSocket() < 0) {
        return -1;
    }

    int sock = createBroadcastSocket();
    if (sock < 0) {
        cleanupSocket();
        return -1;
    }

    std::string broadcastIp = getBroadcastAddress();
    spdlog::info("MACH: Broadcasting server on broadcast IP: {}", broadcastIp);
    struct sockaddr_in broadcastAddr = setupBroadcastAddress(broadcastIp);
    sendBroadcastMessage(sock, broadcastAddr);

    closeSocket(sock);
    cleanupSocket();

    return 0;
}

/**
 * Broadcasts the server to the network on a 5 second interval.
 */
static void _startBroadcastingServer() {
    while (true) {
        broadcastServer();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void startBroadcastingServer() {
    std::thread(_startBroadcastingServer).detach();
}