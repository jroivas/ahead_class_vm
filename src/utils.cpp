#include "utils.h"

#include <arpa/inet.h>
#include <endian.h>

uint16_t read16(uint8_t *p) {
    return ntohs(*(uint16_t*)p);
}

uint32_t read32(uint8_t *p) {
    return ntohl(*(uint32_t*)p);
}

uint64_t read64(uint8_t *p) {
    return be64toh(*(uint64_t*)p);
}
