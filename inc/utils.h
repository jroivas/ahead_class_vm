#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint16_t read16(uint8_t *);
uint32_t read32(uint8_t *);
uint64_t read64(uint8_t *);

namespace utils {
std::vector<std::string> split(const char *str, char c=' ');
}
