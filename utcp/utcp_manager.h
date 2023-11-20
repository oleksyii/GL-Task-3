// utcp_header.h
#ifndef UTCP_MANAGER_H
#define UTCP_MANAGER_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include "checksums.h"
#include "utcp_header.h"

#define SRC_PORT 12345
#define DEST_PORT 54321
#define PORT 54321

// Structure for the packet
struct Packet {
    int sequenceNumber;
    char data[256];
};

class UTCP
{
public:

    UTCP();

    void send_utcp(char* data);
    void send_utcp(Packet& packet);
    void send_ack(int ack);

    char* recv_utcp();
    int recv_ack();

    int Send(char* data);

    int Recv(char* buffer);

private:
    char* packetToCharArray(Packet packet);
    Packet charArrayToPacket(const char* charArray);
    std::vector<int> send_with_acknowledgement(std::vector<Packet> packets);

};

#endif