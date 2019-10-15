#include <iostream>
#include <random>

#include <rs_support.hpp>
#include <trit_vector.hpp>

static constexpr uint64_t NUM_TRITS = 5000000;

std::vector<uint8_t> generate_trits() {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint8_t> dist(0, 2);

    std::vector<uint8_t> trits(NUM_TRITS);
    for (uint64_t i = 0; i < NUM_TRITS; ++i) {
        trits[i] = dist(engine);
    }
    return trits;
}

template <uint8_t Trit>
void test_template(const succinctrits::trit_vector& tv) {
    succinctrits::rs_support<Trit> tv_rs(&tv);
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        if (tv[i] != tv_rs[i]) {
            std::cerr << "Error: tv[i] != tv_rs[i] (" << int(tv[i]) << " != " << int(tv_rs[i]) << std::endl;
            return;
        }
    }

    uint64_t rank = 0;
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        uint64_t r = tv_rs.rank(i);
        if (rank != r) {
            std::cerr << "Error: Rank(" << i << ") = " << r << ", but != " << rank << std::endl;
            return;
        }
        if (tv[i] == Trit) {
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

    if (tv.get_num_trits() != trits.size()) {
        std::cerr << "Error: tv.get_num_trits() != trits.size() ("  //
                  << tv.get_num_trits() << " != " << trits.size() << std::endl;
        return 1;
    }
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        if (tv[i] != trits[i]) {
            std::cerr << "Error: tv[i] != trits[i] (" << int(tv[i]) << " != " << int(trits[i]) << std::endl;
            return 1;
        }
    }

    test_template<0>(tv);
    test_template<1>(tv);
    test_template<2>(tv);

    return 0;
}