#include "hex_utils.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

string byteToHex(unsigned char byte) {
    stringstream ss;
    ss << uppercase << hex << setfill('0') << setw(2) << static_cast<int>(byte);
    return ss.str();
}

unsigned char hexToByte(const string& hex) {
    if (hex.length() != 2) {
        throw invalid_argument("hex-строка должна иметь длину 2");
    }
    
    auto hexVal = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    };
    
    int high = hexVal(hex[0]);
    int low = hexVal(hex[1]);
    
    if (high == -1 || low == -1) {
        throw invalid_argument(string("некорректная hex-пара: ") + hex);
    }
    
    return static_cast<unsigned char>((high << 4) | low);
}