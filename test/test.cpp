#include <iostream>
#include <random>

#include <rank_support.hpp>
#include <trit_vector.hpp>

static constexpr uint32_t NUM_TRITS = 5000000;

std::vector<uint8_t> generate_trits() {
    std::random_device rd;
    std::vector<uint8_t> trits(NUM_TRITS);
    for (uint32_t i = 0; i < NUM_TRITS; ++i) {
        trits[i] = uint8_t(rd() % 3);
    }
    return trits;
}

template <uint8_t Trit>
void test_template(const succinctrits::trit_vector& tv, const std::vector<uint8_t>& orig_trits) {
    succinctrits::rank_support<Trit> tv_r(&tv);
    uint32_t rank = 0;
    for (uint32_t i = 0; i < NUM_TRITS; ++i) {
        if (rank != tv_r.rank(i)) {
            std::cerr << "NG: " << rank << " != " << tv_r.rank(i) << std::endl;
            return;
        }
        if (orig_trits[i] == Trit) {
            ++rank;
        }
    }
    std::cerr << "OK!" << std::endl;
}

int main() {
    auto orig_trits = generate_trits();
    succinctrits::trit_vector tv(orig_trits.begin(), orig_trits.size());

    test_template<0>(tv, orig_trits);
    test_template<1>(tv, orig_trits);
    test_template<2>(tv, orig_trits);
}