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

std::vector<std::string> utils::split(const char *str, char c)
{
    std::vector<std::string> result;
    do
    {
        const char *begin = str;
        while(*str != c && *str) str++;
        result.push_back(std::string(begin, str));
    } while (0 != *str++);

    return result;
}
