#include "utcp_manager.h"


char* UTCP::packetToCharArray(Packet packet)
{
    // Allocate memory for the char* array
    char* result = new char[sizeof(Packet)];

    // Copy the contents of the Packet to the char* array
    std::memcpy(result, &packet, sizeof(Packet));
    printf("packetToCharArray packet: %d %s\n", packet.sequenceNumber, packet.data);
    Packet tmpPack;
    std::memcpy(&tmpPack, result, sizeof(Packet));
    printf("packetToCharArray result: %d %s\n", tmpPack.sequenceNumber, tmpPack.data);

    return result;
}

// Function to convert a char* array back to a Packet
Packet UTCP::charArrayToPacket(const char* charArray) 
{
    Packet result;
    char* tmp = (char*)malloc(sizeof(charArray));
    // std::cout << "charArray: " << charArray << std::endl;
    printf("charArrayToPacket first char: %c last char: %c\n", charArray[0], charArray[255]);
    // Copy the contents of the char* array to the Packet
    std::memcpy(&result, charArray, sizeof(Packet));

     // Convert the char array to a 16-bit unsigned integer in host byte order
    uint16_t value;
    std::memcpy(&value, result.data, sizeof(result.data));

    // Apply ntohs to convert the value to host byte order
    value = ntohs(value);
    char tmpres[256];
    std::memcpy(&tmpres, &value, sizeof(value));
    printf("charArrayToPacket Converted ntohs: %s", tmpres );

    printf("from inside charArrayToPacket, the packet is: %d %s\n", result.sequenceNumber, result.data);

    return result;
}

// Function to split data into packets
std::vector<Packet> splitDataIntoPackets(const char* data, size_t dataSize, size_t maxPacketSize) {
    std::vector<Packet> packets;
    printf("#1 splitDataIntoPackets the data passed is: %s\n", data);

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

        printf("Packet%d : %s char*:%s\n", i, pck.data, packet);

        delete[] packet;
    }

    return packets;
}

// Comparator function for sorting based on sequenceNumber
static bool compareSequenceNumber(const Packet& a, const Packet& b) {
    return a.sequenceNumber < b.sequenceNumber;
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
       char* tmp;
    
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
    char packet[40000];
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
    tmp = packetToCharArray(inputPacket);
    std::memcpy(data, tmp, sizeof(Packet));
    

    Packet tmpPack;
    std::memcpy(&tmpPack, data, sizeof(Packet));
    printf("send_utcp data: %d %s\n", tmpPack.sequenceNumber, tmpPack.data);

   
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
    delete[] tmp;
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

Packet UTCP::recv_utcp()
{
    int sockfd;

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set a timeout for the socket using setsockopt
    struct timeval timeout;
    timeout.tv_sec = 5; // Set the timeout to 5 seconds
    timeout.tv_usec = 1; // Set the timeout to 1 millisecond
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt");
        // return -1;
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        char buffer[40000];

        // Use select to wait for data or timeout
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        int select_result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);

        if (select_result == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (select_result == 0)
        {
            std::cerr << "Timeout occurred recv_utcp. No data received within the specified time." << std::endl;
            close(sockfd);
            return {-1, ""};
        }

        ssize_t packet_size = recv(sockfd, buffer, sizeof(buffer), 0);
        if (packet_size == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // Extract IP header
        struct iphdr *ip_header = (struct iphdr *)buffer;

        // Check if it's a UDP packet
        if (ip_header->protocol == IPPROTO_UDP)
        {
            // printf("\n\nThe contents of buffer received are: %s\n\n", buffer);
            // Extract UDP header
            struct utcphdr *udp_header = (struct utcphdr *)(buffer + sizeof(struct iphdr));
            if (ntohs(udp_header->dest) == PORT)
            {
                std::cout << ntohs(udp_header->dest) << std::endl;
                // Extract data (payload)
                char *data = (char*)(buffer + sizeof(struct iphdr) + sizeof(struct utcphdr));
                // printf("from inside recv_utcp after extracting the data is: %s\n", data);   

                // Verify the checksum
                if (verify_checksum(data, sizeof(data), udp_header->check))
                {
                    printf("Checksum is valid. Packet is not broken.\n");
                }
                else
                {
                    printf("Checksum is invalid. Packet may be broken.\n");
                    return {-1, ""};
                }

                Packet receivedPacket = charArrayToPacket(data);
                printf("from inside recv_utcp, the packet is: %s\n", receivedPacket.data);
                send_ack(receivedPacket.sequenceNumber);

                // Print received data
                printf("Received UDP packet from %s:%d to %s:%d\n",
                       inet_ntoa(*(struct in_addr *)&ip_header->saddr),
                       ntohs(udp_header->source),
                       inet_ntoa(*(struct in_addr *)&ip_header->daddr),
                       ntohs(udp_header->dest));

                close(sockfd);
                
                return receivedPacket;
                // printf("Data: %s\n", data);
            }
        }
    }
}

int UTCP::recv_ack()
{
    int sockfd;

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // Set a timeout for the socket using setsockopt
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 1; // Set the timeout to 5 sec 0.001 milliseconds
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt");
        // return -1;
        exit(EXIT_FAILURE);
    }

    char buffer[40000];
    ssize_t packet_size;

    // Use select to wait for data or timeout
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    int select_result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);

    if (select_result == -1)
    {
        perror("select");
        exit(EXIT_FAILURE);
    }
    else if (select_result == 0)
    {
        std::cerr << "Timeout occurred recv_ack. No data received within the specified time." << std::endl;
        close(sockfd);
        return -1;
    }

    while (1)
    {

        // Data is ready to be received
        packet_size = recv(sockfd, buffer, sizeof(buffer), 0);
        if (packet_size == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        // Extract IP header
        struct iphdr *ip_header = (struct iphdr *)buffer;

        // Check if it's a UDP packet
        if (ip_header->protocol == IPPROTO_UDP)
        {
            // Extract UDP header
            struct utcphdr *udp_header = (struct utcphdr *)(buffer + sizeof(struct iphdr));
            if (ntohs(udp_header->dest) == PORT)
            {

                // Extract data (payload)
                char *data = buffer + sizeof(struct iphdr) + sizeof(struct utcphdr);

                // Verify the checksum
                if (verify_checksum(data, sizeof(data), udp_header->check))
                {
                    printf("Checksum is valid. Packet is not broken.\n");
                }
                else
                {
                    printf("Checksum is invalid. Packet may be broken.\n");
                    return -1;
                }

                close(sockfd);
                return atoi(data);
            }
        }
    }
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
 * @return size of a data sent
 */
int UTCP::Send(char* data)
{
    printf("The original data is: %s\n\n", data);

    // const char* originalData = "This is a sample message for packet splitting.";
    size_t dataSize = std::strlen(data);
    size_t maxPacketSize = 16; // Set your desired maximum packet size

    // Split data into packets
    std::vector<Packet> packets = splitDataIntoPackets(data, dataSize, maxPacketSize);

    std::vector<int> receivedACK;
    for( Packet packet : packets){
        UTCP::send_utcp(packet);
        receivedACK.push_back(UTCP::recv_ack());
    }
    
    while(receivedACK.size() != packets.size()){
        for(Packet packet : packets){
            if(std::find(receivedACK.begin(), receivedACK.end(), packet.sequenceNumber) == receivedACK.end()){
                UTCP::send_utcp(packet);
                receivedACK.push_back(UTCP::recv_ack());
            }
        }
    }

    int ackEnd = 0;
    Packet end = {-1, ""};
    while (ackEnd != -1)
    {
        UTCP::send_utcp(end);
        ackEnd = UTCP::recv_ack();
    }

    int res = packets.back().sequenceNumber + 16;

    return res;
}

/**
 * @brief Receives data using UDP but aknowledging and re-sending broken packets
 * 
 * @param buffer Buffer to where received data will be stored 
 * @return size of data received 
 */
int UTCP::Recv(char* buffer[])
{
    Packet tmp = {0, ""};
    std::vector<Packet> packets = {};

    do
    {
        tmp = recv_utcp();
        printf("#1 first packet received is: %s", tmp.data);
        packets.push_back(tmp);
    }
    while(tmp.sequenceNumber != -1);

    // Sort the vector based on sequenceNumber
    std::sort(packets.begin(), packets.end(), compareSequenceNumber);

    // Remove duplicates using std::unique
    packets.erase(std::unique(packets.begin(), packets.end(), 
                [](const Packet& a, const Packet& b) {
                    return a.sequenceNumber == b.sequenceNumber;
                }), packets.end());

    std::string res;
    for(Packet packet : packets){
        
        res.append(packet.data);
    }
    std::cout << "\nData from the inside Recv function:\n" << res << std::endl;
    strcpy(*buffer, res.c_str());
    return sizeof(*buffer);
}

