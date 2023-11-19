#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include "utcp/utcp_header.h"

#define PORT 54321

int main() {
    int sockfd;

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    while (1) {
        //Move the whole thing to utcp_manager
        //Here should be only receiving th char* data (payload)
        
         
        char buffer[65536];  // Maximum size for an IPv4 packet
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

            // Extract data (payload)
            char *data = buffer + sizeof(struct iphdr) + sizeof(struct utcphdr);

            // Print received data
            printf("Received UDP packet from %s:%d to %s:%d\n",
                   inet_ntoa(*(struct in_addr *)&ip_header->saddr),
                   ntohs(udp_header->source),
                   inet_ntoa(*(struct in_addr *)&ip_header->daddr),
                   ntohs(udp_header->dest));

            printf("Data: %s\n", data);
        }
    }

    // Close the socket (this part of the code is unreachable in this example)
    close(sockfd);

    return 0;
}
