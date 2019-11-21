#include <iostream>
#include <random>

#include <rs_support.hpp>
#include <compressed_rs_support.hpp>
#include <trit_vector.hpp>

namespace {

static constexpr uint64_t NUM_TRITS = 5000000;
const std::string bar = "****************                ";

std::string get_progress_bar(size_t i, size_t max) {
    auto len = 16 * (float(i+1)/max);
    return bar.substr(16-len, 16);
}

std::vector<uint8_t> generate_trits() {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint8_t> dist(0, 5); // Biased

    std::vector<uint8_t> trits(NUM_TRITS);
    for (uint64_t i = 0; i < NUM_TRITS; ++i) {
        auto val = dist(engine);
        trits[i] = val > 1 ? 0 : val+1;
    }
    return trits;
}

template <uint8_t Trit>
void test_template(const succinctrits::trit_vector& tv) {
    succinctrits::rs_support<Trit> tv_rs(&tv);

    uint64_t rank = 0;
    std::cerr << "rank" << (int)Trit << std::endl;
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        uint64_t r = tv_rs.rank(i);
        if (rank != r) {
            std::cerr << "Error: Rank(" << i << ") = " << r << ", but != " << rank << std::endl;
            return;
        }
        if (tv[i] == Trit) {
            ++rank;
        }
        if (i % 0x1000 == 0) {
            std::cerr << "\r[" << get_progress_bar(i, tv.get_num_trits()) << "]";
        }
    }

    std::cerr << std::endl << "select" << (int)Trit << std::endl;
    for (uint64_t n = 0; n < tv_rs.get_num_target_trits(); ++n) {
        uint64_t i = tv_rs.select(n);
        uint64_t r = tv_rs.rank(i);
        if (r != n) {
            std::cerr << "Error: Select(" << n << ") = " << i << ", but Rank(" << i << ") != " << r << std::endl;
            return;
        }
        if (n % 0x1000 == 0) {
            std::cerr << "\r[" << get_progress_bar(n, tv_rs.get_num_target_trits()) << "]";
        }
    }

    std::cerr << "No Problem!" << std::endl;

    std::cerr << "rs_support<" << (int)Trit << ">+trit_vector size: " << tv_rs.size_in_bytes()+tv.size_in_bytes() << std::endl;
}

template <unsigned Trit>
auto test_compressed_trit(const succinctrits::trit_vector& tv, const succinctrits::compressed_rs_support& tv_rs) {
    uint64_t rank = 0;
    std::cerr << "rank" << (int)Trit << std::endl;
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        uint64_t r = tv_rs.rank<Trit>(i);
        if (rank != r) {
            std::cerr << "Error: Rank(" << i << ") = " << r << ", but != " << rank << std::endl;
            return;
        }
        if (tv[i] == Trit) {
            ++rank;
        }
        if (i % 0x1000 == 0) {
            std::cerr << "\r[" << get_progress_bar(i, tv.get_num_trits()) << "]";
        }
    }

    auto num_target_trits = tv_rs.rank<Trit>(tv_rs.size());
    std::cerr << std::endl << "select" << (int)Trit << std::endl;
    for (uint64_t n = 0; n < num_target_trits; ++n) {
        uint64_t i = tv_rs.select<Trit>(n);
        uint64_t r = tv_rs.rank<Trit>(i);
        if (r != n) {
            std::cerr << "Error: Select(" << n << ") = " << i << ", but Rank(" << i << ") != " << r << std::endl;
            return;
        }
        if (n % 0x1000 == 0) {
            std::cerr << "\r[" << get_progress_bar(n, num_target_trits) << "]";
        }
    }

    std::cerr << "No Problem!" << std::endl;
}

}

int main() {
    auto trits = generate_trits();
    succinctrits::trit_vector tv(trits.begin(), trits.size());

    if (tv.get_num_trits() != trits.size()) {
        std::cerr << "Error: tv.get_num_trits() != trits.size() ("  //
                  << tv.get_num_trits() << " != " << trits.size() << std::endl;
        return 1;
    }
    std::cerr << "trit" << std::endl;
    size_t occ[] = {0,0,0};
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        if (tv[i] != trits[i]) {
            std::cerr << "Error: tv[" << i << "] != trits[" << i << "] (" << int(tv[i]) << " != " << int(trits[i]) << std::endl;
            return 1;
        }
        if (i % 0x1000 == 0) {
            std::cerr << "\r[" << get_progress_bar(i, tv.get_num_trits()) << "]";
        }
        occ[tv[i]]++;
    }
    std::cerr << "No Problem!" << std::endl;
    std::cerr << "trits occurence... {0,1,2} = {" << occ[0] << ", " << occ[1] << ", " << occ[2] << "}" << std::endl;

    test_template<0>(tv);
    test_template<1>(tv);
    test_template<2>(tv);

    std::cerr << std::endl << "compressed" << std::endl;
    succinctrits::compressed_rs_support c_tv_rs(tv);
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        if (tv[i] != c_tv_rs[i]) {
            std::cerr << "Error: tv[" << i << "] != c_tv_rs[" << i << "] (" << int(tv[i]) << " != " << int(c_tv_rs[i]) << std::endl;
            return 1;
        }
        if (i % 0x1000 == 0) {
            std::cerr << "\r[" << get_progress_bar(i, tv.get_num_trits()) << "]";
        }
    }
    std::cerr << "No Problem!" << std::endl;

    test_compressed_trit<0>(tv, c_tv_rs);
    test_compressed_trit<1>(tv, c_tv_rs);
    test_compressed_trit<2>(tv, c_tv_rs);

    std::cerr << "compressed_rs_support size: " << c_tv_rs.size_in_bytes() << std::endl;

    return 0;
}