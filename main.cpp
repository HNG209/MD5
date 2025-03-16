#include<iostream>
#include<vector>
#include<sstream>
#include<iomanip>

using namespace std;

const std::vector<uint32_t> STATES = {
    0xA3F9C7D2,
    0x5B8E2F4A,
    0x9D1FA3E7,
    0x6C4B8D19
};

const std::vector<uint32_t> K = {//K constants
    0x3a4f350c,
    0xa33664f1,
    0x553a51dc,
    0x1339b11e,
    0xdc036618,
    0x8d53aa42,
    0x3ca71ad7,
    0x47a9c83b,
    0xd5c4dee4,
    0x6a44b77c,
    0xfcadeb8f,
    0x599113d5,
    0x3594172b,
    0xe6505c18,
    0xad02927a,
    0x2bddf7c7,
    0xda87d45b,
    0x00f0b66d,
    0x9d35b359,
    0xcd160e20,
    0x2995d4e4,
    0xcb4477e9,
    0xf823226a,
    0xc93e2c88,
    0x316db96e,
    0x31a73ce6,
    0xe275a150,
    0x8202df9b,
    0xc2554359,
    0xed724160,
    0x0467bbb7,
    0xc270074e,
    0x203f0b8d,
    0x57f95ca6,
    0xe5fb3f9e,
    0xc36f9f80,
    0xd5dfc164,
    0xe3aac171,
    0xfd8c0b23,
    0x0eaf636f,
    0x5988db80,
    0xec38c763,
    0x2cf1690e,
    0x4e948808,
    0x150c9806,
    0x36db1548,
    0xebf90440,
    0x50553cde,
    0xf61576a6,
    0xd25086ae,
    0x62b66878,
    0x8df9c4cb,
    0xc1ff4fd3,
    0x3f6dcaa6,
    0xa1f08b81,
    0x1ef34dcb,
    0xaee773f8,
    0x18fb2a09,
    0xf8a73098,
    0x06ed1d8f,
    0x18e2d0bb,
    0x5dfa548b,
    0x2f6b334d,
    0xb196a450
};
const std::vector<int> r = {//rotation constants
    35, 6, 2, 5, 22, 34, 4, 13,
    29, 34, 19, 17, 32, 24, 31, 39,
    22, 46, 13, 17, 28, 23, 60, 45,
    50, 40, 41, 50, 35, 14, 61, 6,
    5, 37, 23, 63, 62, 36, 2, 31,
    27, 38, 38, 42, 5, 10, 5, 38,
    53, 19, 20, 16, 33, 39, 3, 36,
    45, 23, 58, 57, 48, 63, 25, 7
};

std::string hexToString(uint32_t num) {
    std::ostringstream oss;
    oss << std::hex << std::setw(8) << std::setfill('0') << num;
    return oss.str();
}

std::string stringToHex(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        // Convert each character to a 2-digit hex representation
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
    }
    return oss.str();
}

std::string to512Bits(const std::string& input) {
    string hex = stringToHex(input);
    const int totalLength = 128;  // 512 bits = 128 hex characters
    if (hex.length() >= totalLength) {
        return hex.substr(0, totalLength);  // Truncate if longer
    }
    // Pad with zeros if shorter
    return hex + std::string(totalLength - hex.length(), '0');
}


uint32_t rotateLeft(uint32_t value, uint32_t shift) {
    const uint32_t BITS = sizeof(value) * 8;  // 32 bits
    shift %= BITS;  // Avoid overflow
    return (value << shift) | (value >> (BITS - shift));
}

uint32_t redBox(uint32_t input, uint32_t B, int i) {
    int result = (rotateLeft(input + K[i], r[i])) + B;
    return result;
}

uint32_t F(uint32_t B, uint32_t C, uint32_t D, int i) {
    if (i >= 0 && i < 16)
        return (B & C) | (~B & D);
    else if (i >= 16 && i < 32)
        return (D & B) | (~D & C);
    else if (i >= 32 && i < 48)
        return B ^ C ^ D;//^: XOR
    return C ^ (B | ~D);
}

uint32_t combine(uint32_t A, uint32_t B, uint32_t C, uint32_t D, int i, uint32_t iValue) {
    uint32_t sum = A + F(B, C, D, i) + iValue;

    return redBox(sum, B, i);
}

std::string MD5(std::string input) {
    input = to512Bits(input);

    std::vector<uint32_t> segments;
    uint32_t stateA = STATES[0];
    uint32_t stateB = STATES[1];
    uint32_t stateC = STATES[2];
    uint32_t stateD = STATES[3];

    for (size_t i = 0; i < input.length(); i += 8) {
        std::string segmentStr = input.substr(i, 8);
        uint32_t segment;

        std::stringstream ss;
        ss << std::hex << segmentStr;
        ss >> segment;

        segments.push_back(segment);
    }

    uint32_t preStateA = stateA;
    uint32_t prevStateB = stateB;
    uint32_t prevStateC = stateC;
    uint32_t prevStateD = stateD;

    for (int i = 0; i < 64; i++) {

        if (i < 16) {
            stateB = combine(stateA, stateB, stateC, stateD, i, segments[i % 16]);
            stateA = prevStateD;
            stateC = prevStateB;
            stateD = prevStateC;
        }
        else if (i >= 16 && i < 32) {
            stateB = combine(stateA, stateB, stateC, stateD, i, segments[(5 * i + 1) % 16]);
            stateA = prevStateD;
            stateC = prevStateB;
            stateD = prevStateC;
        }
        else if (i >= 32 && i < 48) {
            stateB = combine(stateA, stateB, stateC, stateD, i, segments[(3 * i + 5) % 16]);
            stateA = prevStateD;
            stateC = prevStateB;
            stateD = prevStateC;
        }
        else if (i >= 48 && i < 64) {
            stateB = combine(stateA, stateB, stateC, stateD, i, segments[(7 * i) % 16]);
            stateA = prevStateD;
            stateC = prevStateB;
            stateD = prevStateC;
        }

        preStateA = stateA;
        prevStateB = stateB;
        prevStateC = stateC;
        prevStateD = stateD;
    }

    return hexToString(stateA) + hexToString(stateB) + hexToString(stateC) + hexToString(stateD);
}

int main() {
    string s = "hello";
    cout << "MD5 hashed: " << MD5(s);
	return 0;
}