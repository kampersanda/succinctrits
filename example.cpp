#include <iostream>

#include "rank_support.hpp"
#include "trit_vector.hpp"

int main() {
    std::vector<uint8_t> trits;
    trits.push_back(0);
    trits.push_back(1);
    trits.push_back(0);
    trits.push_back(2);
    trits.push_back(1);
    trits.push_back(2);
    trits.push_back(2);
    trits.push_back(0);
    trits.push_back(1);
    trits.push_back(0);
    trits.push_back(1);
    trits.push_back(2);
    trits.push_back(2);
    trits.push_back(0);

    succinctrits::trit_vector_builder b;
    for (size_t i = 0; i < trits.size(); ++i) {
        b.push_back(trits[i]);
    }

    succinctrits::trit_vector tvec;
    tvec.build(b);

    for (size_t i = 0; i < trits.size(); ++i) {
        std::cout << int(b.get(i)) << " : " << int(tvec.get(i)) << std::endl;
    }

    std::cout << tvec.get_num_trits() << std::endl;
    std::cout << tvec.get_num_0s() << " : " << tvec.get_num_1s() << " : " << tvec.get_num_2s() << std::endl;

    succinctrits::rank_support<2> tvec_rank(&tvec);
    for (size_t i = 0; i < trits.size(); ++i) {
        std::cout << int(b.get(i)) << " : " << tvec_rank.rank(i) << std::endl;
    }
}