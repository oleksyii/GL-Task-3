#include "checksums.h"

// Function to calculate the checksum for a char array (UDP payload)
uint16_t calcculate_checksum_utcp(const char *data, size_t length) {
    uint32_t sum = 0;

    // Sum up the 16-bit words in the data
    for (size_t i = 0; i < length; i += 2) {
        uint16_t word = *(uint16_t *)(data + i);
        sum += word;
    }

    // If the length is odd, add the last byte
    if (length % 2 != 0) {
        sum += (uint16_t)data[length - 1];
    }

    // Perform carry operation
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // Take one's complement of the result
    return (uint16_t)~sum;
}

// Function to verify the checksum for a char array (UDP payload)
bool verify_checksum(const char *data, size_t length, uint16_t checksum) {
    uint16_t calculated_checksum = calcculate_checksum_utcp(data, length);
    return calculated_checksum == checksum;
}