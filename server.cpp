#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include "utcp/utcp_header.h"

#define SRC_PORT 12345
#define DEST_PORT 54321

// Pseudo-header structure for UDP checksum calculation
struct pseudo_header {
    struct in_addr source_address;
    struct in_addr dest_address;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t udp_length;
};

// Function to calculate checksum
uint16_t checksum(uint16_t *ptr, int length) {
    int sum = 0;
    uint16_t oddbyte;
    while (length > 1) {
        sum += *ptr++;
        length -= 2;
    }
    if (length == 1) {
        oddbyte = 0;
        *((uint8_t*)&oddbyte) = *(uint8_t*)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t)(~sum);
}

int main() {
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
    strcpy(data, "Hello, UDP!");

    // Calculate UDP checksum
    struct pseudo_header pseudo_hdr;
    pseudo_hdr.source_address.s_addr = inet_addr("192.168.1.2");  // Source IP address
    pseudo_hdr.dest_address.s_addr = dest_addr.sin_addr.s_addr;
    pseudo_hdr.placeholder = 0;
    pseudo_hdr.protocol = IPPROTO_UDP;
    pseudo_hdr.udp_length = htons(sizeof(struct utcphdr) + strlen("Hello, UDP!"));

    int pseudo_length = sizeof(struct pseudo_header) + sizeof(struct utcphdr) + strlen("Hello, UDP!");
    char *pseudo_packet = (char*)malloc(pseudo_length);
    memcpy(pseudo_packet, (char *)&pseudo_hdr, sizeof(struct pseudo_header));
    memcpy(pseudo_packet + sizeof(struct pseudo_header), udp_header, sizeof(struct utcphdr) + strlen("Hello, UDP!"));

    udp_header->check = checksum((uint16_t *)pseudo_packet, pseudo_length);

    // Send the packet
    if (sendto(sockfd, packet, ip_header->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    printf("Packet sent.\n");

    // Clean up
    close(sockfd);
    free(pseudo_packet);

    return 0;
}
