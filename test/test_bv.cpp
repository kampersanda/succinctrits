#include <iostream>
#include <random>

#include "compressed_rs_support.hpp"

namespace {
constexpr size_t SIZE = 5000000;
const std::string bar = "****************                ";
}

int main() {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint8_t> dist(0, 1);
    std::vector<bool> bits(SIZE);
    for (uint64_t i = 0; i < SIZE; ++i) {
        bits[i] = dist(engine);
    }

    succinctrits::bit_vector bv(SIZE);
    std::vector<size_t> ranks(SIZE), selects;
    size_t cnt = 0;
    for (size_t i = 0; i < SIZE; i++) {
        ranks[i] = cnt;
        if (bits[i] == 1) {
            cnt++;
            selects.push_back(i);
        }
        bv[i] = bits[i];
    }
    succinctrits::rs_support_bv rs_bv(&bv);

    std::cout << "exact" << std::endl;
    for (size_t i = 0; i < SIZE; i++) {
        if (bv[i] != bits[i]) {
            std::cerr << "bv[" << i << "] != bits[" << i << "]: " << bool(bv[i]) << " != " << bool(bits[i]) << std::endl;
            return 1;
        }
        if (i % 0x1000 == 0) {
            auto len = 16 * (float(i+1) / SIZE);
            std::cout << "\r[" << bar.substr(16-len, 16) << "]";
        }
    }
    std::cout << "ok" << std::endl;

    std::cout << "rank" << std::endl;
    for (size_t i = 0; i < SIZE; i++) {
        if (rs_bv.rank(i) != ranks[i]) {
            std::cerr << "Failed bv.rank(" << i << "): " << rs_bv.rank(i) << " != " << ranks[i] << std::endl;
            return 1;
        }
        if (i % 0x1000 == 0) {
            auto len = 16 * (float(i+1) / SIZE);
            std::cout << "\r[" << bar.substr(16-len, 16) << "]";
        }
    }
    std::cout << "ok" << std::endl;

    std::cout << "select" << std::endl;
    for (size_t i = 0; i < selects.size(); i++) {
        if (rs_bv.select(i) != selects[i]) {
            std::cerr << "Failed bv.select(" << i << "): " << rs_bv.select(i) << " != " << selects[i] << std::endl;
            return 1;
        }
        if (i % 0x1000 == 0) {
            auto len = 16 * (float(i+1) / selects.size());
            std::cout << "\r[" << bar.substr(16-len, 16) << "]";
        }
    }
    std::cout << "ok" << std::endl;

}
