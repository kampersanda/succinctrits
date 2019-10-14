#include <iostream>
#include <random>

#include <rs_support.hpp>
#include <trit_vector.hpp>

int main() {
    std::vector<uint8_t> trits = {1, 0, 1, 2, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 2, 1, 2, 0, 1};
    succinctrits::trit_vector tv(trits.begin(), trits.size());

    std::cout << "addr: ";
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        std::cout << i % 10 << ' ';
    }
    std::cout << std::endl << "trit: ";
    for (uint64_t i = 0; i < tv.get_num_trits(); ++i) {
        std::cout << uint32_t(tv[i]) << ' ';
    }
    std::cout << std::endl;

    succinctrits::rs_support<0> tv_rs_0(&tv);
    succinctrits::rs_support<1> tv_rs_1(&tv);
    succinctrits::rs_support<2> tv_rs_2(&tv);

    std::cout << "Rank_0(6)   = " << tv_rs_0.rank(6) << std::endl;
    std::cout << "Rank_1(10)  = " << tv_rs_1.rank(10) << std::endl;
    std::cout << "Rank_2(12)  = " << tv_rs_2.rank(12) << std::endl;
    std::cout << "Select_0(3) = " << tv_rs_0.select(3) << std::endl;
    std::cout << "Select_1(5) = " << tv_rs_1.select(5) << std::endl;
    std::cout << "Select_2(0) = " << tv_rs_2.select(0) << std::endl;

    return 0;
}