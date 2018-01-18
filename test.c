#include <stdio.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned int  u32;

u32 FIRST_LEN[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1
};

u8 MASK[] = {
    0xFF, 0xFF, 0x1F, 0xF, 0x7
};

#define InvalidCodePoint 0xFFFD

// WARNING: this function cannot handle a buffer where `len = 0`
u32 DecodeCodePoint(int *cpLen, u8 *buffer, size_t len) {
    u8 b0 = buffer[0];
    int l = FIRST_LEN[b0];
    int val = (int)(b0 & MASK[l]);

    for (int i=1; i < l; i += 1) {
        val = (val << 6) | (int)(buffer[i] & 0x3f);
    }

    if (cpLen)
        *cpLen = l;
    return val;
}

// WARNING: this function assumes that buffer is >= 4 bytes
u32 EncodeCodePoint(u8 * const buffer, const u32 cp) {
    if (cp <= 0x7F) {
        buffer[0] = cp;
        return 1;
    }

    if (cp <= 0x7FF) {
        buffer[0] = 0xC0 | (cp >> 6);
        buffer[1] = 0x80 | (cp & 0x3F);
        return 2;
    }

    if (cp <= 0xFFFF) {
        buffer[0] = 0xE0 | (cp >> 12);
        buffer[1] = 0x80 | ((cp >> 6) & 0x3F);
        buffer[2] = 0x80 | (cp & 0x3F);
        return 3;
    }

    if (cp <= 0x10FFFF) {
        buffer[0] = 0xF0 | (cp >> 18);
        buffer[1] = 0x80 | ((cp >> 12) & 0x3F);
        buffer[2] = 0x80 | ((cp >> 6) & 0x3F);
        buffer[3] = 0x80 | (cp & 0x3F);
        return 4;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Expected a unicode byte sequence\n");
        return 1;
    }

    size_t slen = strlen(argv[1]);
    int cpLen;
    u32 cp;
    cp = DecodeCodePoint(&cpLen, (u8 *)argv[1], slen);
    printf("The utf-8 to CP is '%d' bytes large and has the value: 0x%x\n", cpLen, cp);

    u8 buff[4];
    cpLen = EncodeCodePoint(&buff[0], cp);
    printf("CP encoded back into utf-8: `%.*s` is %d bytes long\n", cpLen, buff, cpLen);

    return 0;
}
