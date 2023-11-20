// #include "utcp/utcp_manager.h"
#include "utcp/utcp_manager.h"



// // Pseudo-header structure for UDP checksum calculation
// struct pseudo_header {
//     struct in_addr source_address;
//     struct in_addr dest_address;
//     uint8_t placeholder;
//     uint8_t protocol;
//     uint16_t udp_length;
// };

// // Function to calculate checksum
// uint16_t checksum(uint16_t *ptr, int length) {
//     int sum = 0;
//     uint16_t oddbyte;
//     while (length > 1) {
//         sum += *ptr++;
//         length -= 2;
//     }
//     if (length == 1) {
//         oddbyte = 0;
//         *((uint8_t*)&oddbyte) = *(uint8_t*)ptr;
//         sum += oddbyte;
//     }
//     sum = (sum >> 16) + (sum & 0xFFFF);
//     sum += (sum >> 16);
//     return (uint16_t)(~sum);
// }

int main() {
    UTCP* utcp_manager = new UTCP();
    
    char* data = utcp_manager->recv_utcp();
    printf("Data: %s\n", data);
    delete utcp_manager;

    return 0;
}
