#ifndef UTCPHDR_H
#define UTCPHDR_H

#include <stdint.h>

struct utcphdr {
    uint16_t source;      // Source port
    uint16_t dest;        // Destination port
    uint16_t len;         // UDP packet length (header + data)
    uint16_t check;       // UDP checksum
};

#endif