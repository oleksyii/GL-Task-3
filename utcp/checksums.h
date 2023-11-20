#ifndef CHECKSUMS_H
#define CHECKSUMS_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// Function to calculate the checksum for a char array (UDP payload)
uint16_t calcculate_checksum_utcp(const char *data, size_t length);

// Function to verify the checksum for a char array (UDP payload)
bool verify_checksum(const char *data, size_t length, uint16_t checksum);
#endif