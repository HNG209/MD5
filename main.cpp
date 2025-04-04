#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>

using namespace std;

const uint32_t INIT_STATE[4] = {
    0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476
};

const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

const int R[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

uint32_t leftRotate(uint32_t x, int c) {
    return (x << c) | (x >> (32 - c));
}

uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (~x & z); }
uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (x & z) | (y & ~z); }
uint32_t H(uint32_t x, uint32_t y, uint32_t z) { return x ^ y ^ z; }
uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return y ^ (x | ~z); }

void processBlock(uint32_t state[4], const uint8_t block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t M[16];

    for (int i = 0; i < 16; ++i) {
        M[i] = *(uint32_t*)&block[i * 4];
    }

    for (int i = 0; i < 64; ++i) {
        uint32_t f, g;
        if (i < 16) {
            f = F(b, c, d);
            g = i;
        }
        else if (i < 32) {
            f = G(b, c, d);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48) {
            f = H(b, c, d);
            g = (3 * i + 5) % 16;
        }
        else {
            f = I(b, c, d);
            g = (7 * i) % 16;
        }

        uint32_t temp = d;
        d = c;
        c = b;
        b += leftRotate(a + f + K[i] + M[g], R[i]);
        a = temp;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

string md5(const string& input) {
    uint32_t state[4];
    memcpy(state, INIT_STATE, sizeof(state));

    //add từng byte của input và add thêm bit 1 vào đầu byte sau
    vector<uint8_t> padded;
    padded.insert(padded.end(), input.begin(), input.end());
    padded.push_back(0x80);// 10000000

    //pad các byte còn lại(trừ 8 byte cuối) bằng các byte 0(0x00)
    while ((padded.size() + 8) % 64 != 0) {
        padded.push_back(0);
    }

    //tính toán size của input và pad vào 8 byte cuối
    uint64_t inputLength = input.size() * 8; //ví dụ input có 5 byte -> length = 5 * 8 = 40
                                             //40 = 0x28
    for (int i = 0; i < 8; ++i) {
        //8 byte còn lại = (0x28 0x00 0x00 0x00 0x00 0x00 0x00 0x00)
        padded.push_back((inputLength >> (8 * i)));
    }

    for (size_t i = 0; i < padded.size(); i += 64) {
        processBlock(state, &padded[i]);
    }

    ostringstream result;
    for (int i = 0; i < 4; ++i) {
        result << hex << setw(8) << setfill('0') << state[i];
    }
    return result.str();
}

int main() {
    string input = "hello";
    cout << "MD5: " << md5(input) << endl;
    return 0;
}
