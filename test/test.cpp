#include <iostream>
#include <random>

#include <rs_support.hpp>
#include <trit_vector.hpp>

static constexpr uint64_t NUM_TRITS = 5000000;

std::vector<uint8_t> generate_trits() {
    std::random_device rd;
    std::vector<uint8_t> trits(NUM_TRITS);
    for (uint64_t i = 0; i < NUM_TRITS; ++i) {
        trits[i] = uint8_t(rd() % 3);
    }
    return trits;
}

template <uint8_t Trit>
void test_template(const succinctrits::trit_vector& tv, const std::vector<uint8_t>& trits) {
    succinctrits::rs_support<Trit> tv_rs(&tv);

    uint64_t rank = 0;
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        uint64_t r = tv_rs.rank(i);
        if (rank != r) {
            std::cerr << "Error: Rank(" << i << ") = " << r << ", but != " << rank << std::endl;
            return;
        }
        if (trits[i] == Trit) {
            ++rank;
        }
    }

    for (uint64_t n = 0; n < tv_rs.get_num_target_trits(); ++n) {
        uint64_t i = tv_rs.select(n);
        uint64_t r = tv_rs.rank(i);
        if (r != n) {
            std::cerr << "Error: Select(" << n << ") = " << i << ", but Rank(" << i << ") != " << r << std::endl;
            return;
        }
    }

    std::cerr << "No Problem!" << std::endl;
}

int main() {
    auto trits = generate_trits();
    succinctrits::trit_vector tv(trits.begin(), trits.size());

    test_template<0>(tv, trits);
    test_template<1>(tv, trits);
    test_template<2>(tv, trits);
}