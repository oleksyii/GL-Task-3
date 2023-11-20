#include "utcp_manager.h"


char* UTCP::packetToCharArray(Packet packet)
{
    // Allocate memory for the char* array
    char* result = new char[sizeof(Packet)];

    // Copy the contents of the Packet to the char* array
    std::memcpy(result, &packet, sizeof(Packet));

    return result;
}

// Function to convert a char* array back to a Packet
Packet UTCP::charArrayToPacket(const char* charArray) 
{
    Packet result;

    // Copy the contents of the char* array to the Packet
    std::memcpy(&result, charArray, sizeof(Packet));

    return result;
}

// Function to split data into packets
std::vector<Packet> splitDataIntoPackets(const char* data, size_t dataSize, size_t maxPacketSize) {
    std::vector<Packet> packets;

    for (size_t i = 0; i < dataSize; i += maxPacketSize) {
        size_t packetSize = std::min(maxPacketSize, dataSize - i);

        // Allocate memory for the packet
        char* packet = new char[packetSize + 1]; // +1 for null terminator
        std::memcpy(packet, data + i, packetSize);
        packet[packetSize] = '\0'; // Null-terminate the packet

        // Add the packet to the vector
        Packet pck;
        pck.sequenceNumber = i;
        strcpy(pck.data, packet);

        packets.push_back(pck);
    }

    return packets;
}


UTCP::UTCP() {};

void UTCP::send_utcp(char* input)
{
    int sockfd;
    
    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options (if needed)
    int one = 1;
    const int *val = &one;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Prepare destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);

    }

    // Allocate memory for packet
    char packet[1024];
    memset(packet, 0, sizeof(packet));

    // IP header
    struct iphdr *ip_header = (struct iphdr *)packet;
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct utcphdr) + strlen("Hello, UDP!");
    ip_header->id = htons(12345);
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_UDP;
    ip_header->saddr = inet_addr("192.168.1.2");  // Source IP address (change as needed)
    ip_header->daddr = dest_addr.sin_addr.s_addr;

    // UDP header
    struct utcphdr *udp_header = (struct utcphdr *)(packet + sizeof(struct iphdr));
    udp_header->source = htons(SRC_PORT);
    udp_header->dest = dest_addr.sin_port;
    udp_header->len = htons(sizeof(struct utcphdr) + strlen("Hello, UDP!"));
    udp_header->check = 0;  // Checksum calculation comes next

    // Data (payload)
    char *data = packet + sizeof(struct iphdr) + sizeof(struct utcphdr);
    strcpy(data, input);

   
    // udp_header->check = checksum((uint16_t *)pseudo_packet, pseudo_length);
    udp_header->check = calcculate_checksum_utcp(data, sizeof(data));

    // Send the packet
    if (sendto(sockfd, packet, ip_header->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent.\n");

    // Clean up
    close(sockfd);
}

void UTCP::send_utcp(Packet& inputPacket)
{
       int sockfd;
    
    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options (if needed)
    int one = 1;
    const int *val = &one;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Prepare destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for packet
    char packet[1024];
    memset(packet, 0, sizeof(packet));

    // IP header
    struct iphdr *ip_header = (struct iphdr *)packet;
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct utcphdr) + strlen("Hello, UDP!");
    ip_header->id = htons(12345);
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_UDP;
    ip_header->saddr = inet_addr("192.168.1.2");  // Source IP address (change as needed)
    ip_header->daddr = dest_addr.sin_addr.s_addr;

    // UDP header
    struct utcphdr *udp_header = (struct utcphdr *)(packet + sizeof(struct iphdr));
    udp_header->source = htons(SRC_PORT);
    udp_header->dest = dest_addr.sin_port;
    udp_header->len = htons(sizeof(struct utcphdr) + strlen("Hello, UDP!"));
    udp_header->check = 0;  // Checksum calculation comes next

    // Data (payload)
    char *data = packet + sizeof(struct iphdr) + sizeof(struct utcphdr);
    char* tmp = packetToCharArray(inputPacket);
    strcpy(data, tmp);
    delete[] tmp;

   
    // udp_header->check = checksum((uint16_t *)pseudo_packet, pseudo_length);
    udp_header->check = calcculate_checksum_utcp(data, sizeof(data));

    // Send the packet
    if (sendto(sockfd, packet, ip_header->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent.\n");

    // Clean up
    close(sockfd);
}

void UTCP::send_ack(int ack)
{
        int sockfd;
    
    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options (if needed)
    int one = 1;
    const int *val = &one;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Prepare destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for packet
    char packet[1024];
    memset(packet, 0, sizeof(packet));

    // IP header
    struct iphdr *ip_header = (struct iphdr *)packet;
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct utcphdr) + strlen("Hello, UDP!");
    ip_header->id = htons(12345);
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_UDP;
    ip_header->saddr = inet_addr("192.168.1.2");  // Source IP address (change as needed)
    ip_header->daddr = dest_addr.sin_addr.s_addr;

    // UDP header
    struct utcphdr *udp_header = (struct utcphdr *)(packet + sizeof(struct iphdr));
    udp_header->source = htons(SRC_PORT);
    udp_header->dest = dest_addr.sin_port;
    udp_header->len = htons(sizeof(struct utcphdr) + strlen("Hello, UDP!"));
    udp_header->check = 0;  // Checksum calculation comes next

    // Data (payload)
    char *data = packet + sizeof(struct iphdr) + sizeof(struct utcphdr);
    strcpy(data, std::to_string(ack).c_str());

   
    // udp_header->check = checksum((uint16_t *)pseudo_packet, pseudo_length);
    udp_header->check = calcculate_checksum_utcp(data, sizeof(data));

    // Send the packet
    if (sendto(sockfd, packet, ip_header->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    printf("ACK packet %d is sent.\n", ack);

    // Clean up
    close(sockfd);
}

char* UTCP::recv_utcp()
{
        int sockfd;

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    while (1) {

        char buffer[1024];  
        ssize_t packet_size = recv(sockfd, buffer, sizeof(buffer), 0);
        if (packet_size == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // Extract IP header
        struct iphdr *ip_header = (struct iphdr *)buffer;

        // Check if it's a UDP packet
        if (ip_header->protocol == IPPROTO_UDP) {
            // Extract UDP header
            struct utcphdr *udp_header = (struct utcphdr *)(buffer + sizeof(struct iphdr));
            if(ntohs(udp_header->dest) == PORT)
            {

            // std::cout << ntohs(udp_header->dest) << std::endl;
            // std::cout << udp_header->dest << std::endl;


            // Extract data (payload)
            char* data = buffer + sizeof(struct iphdr) + sizeof(struct utcphdr);



            // Verify the checksum
            if (verify_checksum(data, sizeof(data), udp_header->check)) {
                printf("Checksum is valid. Packet is not broken.\n");
            } else {
                printf("Checksum is invalid. Packet may be broken.\n");
                return nullptr;
            }

            Packet receivedPacket = charArrayToPacket(data);
            send_ack(receivedPacket.sequenceNumber);

            // Print received data
            printf("Received UDP packet from %s:%d to %s:%d\n",
                   inet_ntoa(*(struct in_addr *)&ip_header->saddr),
                   ntohs(udp_header->source),
                   inet_ntoa(*(struct in_addr *)&ip_header->daddr),
                   ntohs(udp_header->dest));

            return data;
            // printf("Data: %s\n", data);
            }

        }
    }

    // Close the socket (this part of the code is unreachable in this example)
    close(sockfd);
}

int UTCP::recv_ack()
{
            int sockfd;

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    while (1) {
        //Move the whole thing to utcp_manager
        //Here should be only receiving th char* data (payload)
        
         
        char buffer[1024];  
        ssize_t packet_size = recv(sockfd, buffer, sizeof(buffer), 0);
        if (packet_size == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // Extract IP header
        struct iphdr *ip_header = (struct iphdr *)buffer;

        // Check if it's a UDP packet
        if (ip_header->protocol == IPPROTO_UDP) {
            // Extract UDP header
            struct utcphdr *udp_header = (struct utcphdr *)(buffer + sizeof(struct iphdr));
            if(ntohs(udp_header->dest) == PORT)
            {

            // Extract data (payload)
            char* data = buffer + sizeof(struct iphdr) + sizeof(struct utcphdr);



            // Verify the checksum
            if (verify_checksum(data, sizeof(data), udp_header->check)) {
                printf("Checksum is valid. Packet is not broken.\n");
            } else {
                printf("Checksum is invalid. Packet may be broken.\n");
                return -1;
            }

            return atoi(data);

            }

        }
    }

    // Close the socket (this part of the code is unreachable in this example)
    close(sockfd);
}

std::vector<int> UTCP::send_with_acknowledgement(std::vector<Packet> packets)
{
    std::vector<int> receivedACK;
    for( Packet packet : packets){
        UTCP::send_utcp(packet);
        receivedACK.push_back(UTCP::recv_ack());
    }

    return receivedACK;
}

/**
 * @brief Sends data using UDP but aknowledging and re-sending broken packets
 * 
 * @param data Data to be sent
 * @return size of a packet sent
 */
int UTCP::Send(char* data)
{
    // const char* originalData = "This is a sample message for packet splitting.";
    size_t dataSize = std::strlen(data);
    size_t maxPacketSize = 256; // Set your desired maximum packet size

    // Split data into packets
    std::vector<Packet> packets = splitDataIntoPackets(data, dataSize, maxPacketSize);

    std::vector<int> receivedACK;
    // for( Packet packet : packets){
    //     UTCP::send_utcp(packet);
    //     receivedACK.push_back(UTCP::recv_ack());
    // }
    
    while(receivedACK.size() != packets.size()){
        for(Packet packet : packets){
            if(std::find(receivedACK.begin(), receivedACK.end(), packet.sequenceNumber) == receivedACK.end()){
                UTCP::send_utcp(packet);
                receivedACK.push_back(UTCP::recv_ack());
            }
        }
    }


    // Clean up allocated memory
    for (Packet packet : packets) {
        delete[] packet.data;
    }
}

/**
 * @brief Receives data using UDP but aknowledging and re-sending broken packets
 * 
 * @param buffer Buffer to where received data will be stored 
 * @return size of a packet received 
 */
int UTCP::Recv(char* buffer)
{

}