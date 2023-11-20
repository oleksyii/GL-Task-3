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
// a new line of code



int main() {
    UTCP* mgr = new UTCP();
    mgr->Send("Hello UDP! Here Comes your saviour, UTCP! It is soooo cool, that it can check for broken packets and do acknowledgement!");
    delete mgr;

    return 0;
}
