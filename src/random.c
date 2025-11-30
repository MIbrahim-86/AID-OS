#include "random.h"

static uint32_t next = 1;

void srand(uint32_t seed) {
    next = seed;
}

uint32_t rand() {
    next = next * 1103515245 + 12345;
    return (uint32_t)(next / 65536) % 32768;
}