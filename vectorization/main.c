/*
  76b3ed7cadf5c206dfeb4d78
  50aebb97493997241a6fb9bb
  c77e19010f79caebeebde3f2
  3aa846fb18afd7416e2a2815
  28a7f3823be03d9343c62910
  e89f9ff33f94771c90163dd2
  6057eac40dd740cde0f586af
  9351cafddb63750f03d6386d
*/

#include <stdint.h>
#include <stdio.h>
#include "crc32.h"

static const uint32_t vdata[8 * 12] = {
    0x76, 0x50, 0xc7, 0x3a, 0x28, 0xe8, 0x60, 0x93,
    0xb3, 0xae, 0x7e, 0xa8, 0xa7, 0x9f, 0x57, 0x51,
    0xed, 0xbb, 0x19, 0x46, 0xf3, 0x9f, 0xea, 0xca,
    0x7c, 0x97, 0x01, 0xfb, 0x82, 0xf3, 0xc4, 0xfd,
    0xad, 0x49, 0x0f, 0x18, 0x3b, 0x3f, 0x0d, 0xdb,
    0xf5, 0x39, 0x79, 0xaf, 0xe0, 0x94, 0xd7, 0x63,
    0xc2, 0x97, 0xca, 0xd7, 0x3d, 0x77, 0x40, 0x75,
    0x06, 0x24, 0xeb, 0x41, 0x93, 0x1c, 0xcd, 0x0f,
    0xdf, 0x1a, 0xee, 0x6e, 0x43, 0x90, 0xe0, 0x03,
    0xeb, 0x6f, 0xbd, 0x2a, 0xc6, 0x16, 0xf5, 0xd6,
    0x4d, 0xb9, 0xe3, 0x28, 0x29, 0x3d, 0x86, 0x38,
    0x78, 0xbb, 0xf2, 0x15, 0x10, 0xd2, 0xaf, 0x6d,
};

struct zc_key {
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
};

#define KEY0 0x12345678
#define KEY1 0x23456789
#define KEY2 0x34567890
#define MULT 134775813u

/* static uint32_t k0 = KEY0; */
/* static uint32_t k1 = KEY1; */
/* static uint32_t k2 = KEY2; */

static inline uint32_t decrypt_byte(uint32_t k)
{
    uint32_t tmp =  (k | 2) & 0xffff;
    return ((tmp * (tmp ^ 1)) >> 8) & 0xff;
}

static inline void update_keys(char c, uint32_t *k0, uint32_t *k1, uint32_t *k2)
{
   *k0 = crc32(*k0, c);
   *k1 = (*k1 + (*k0 & 0x000000ff)) * MULT + 1;
   *k2 = crc32(*k2, *k1 >> 24);
}

static inline void init_encryption_keys(const char *pw, uint32_t *k0, uint32_t *k1, uint32_t *k2)
{
    size_t i = 0;
    *k0 = KEY0;
    *k1 = KEY1;
    *k2 = KEY2;
    while (pw[i] != '\0') {
        update_keys(pw[i], k0, k1, k2);
        ++i;
    }
}

int main(int argc, char *argv[])
{
    uint32_t rdata[8 * 12];
    uint32_t b0, b1, b2;
    uint32_t k0[8], k1[8], k2[8 * 12];

    init_encryption_keys(argv[1], &b0, &b1, &b2);

    for (uint32_t i = 0; i < 8; ++i) {
        k0[i] = b0;
        k1[i] = b1;
        k2[i] = b2;
    }

    for (uint32_t i = 0; i < 12; ++i) {

        for (uint32_t j = 0; j < 8; ++j) {
            rdata[i * 8 + j] = decrypt_byte(k2[i * 8 + j]) ^ vdata[i * 8 + j];
        }

        for (uint32_t j = 0; j < 8; ++j) {
            update_keys(rdata[i * 8 + j], &k0[j], &k1[j], &k2[j]);
        }
    }

    printf("0x%x, 0x%x, 0x%x\n", k0[7], k1[7], k2[7]);
    return 0;
}
