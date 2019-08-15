#include <iostream>

void gen_lut(uint8_t trit) {
    std::cout << "uint8_t LUT_" << int(trit) << "[243] = {";
    for (uint8_t tryte = 0; tryte < 243; ++tryte) {
        int n = 0;
        if (uint8_t(tryte % 3) == trit) ++n;
        if (uint8_t(tryte / 3 % 3) == trit) ++n;
        if (uint8_t(tryte / 9 % 3) == trit) ++n;
        if (uint8_t(tryte / 27 % 3) == trit) ++n;
        if (uint8_t(tryte / 81 % 3) == trit) ++n;
        std::cout << n << ',';
    }
    std::cout << "};\n";
}

int main() {
    gen_lut(0);
    gen_lut(1);
    gen_lut(2);
}