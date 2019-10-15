#include <iostream>
#include <random>

#include <rs_support.hpp>
#include <trit_vector.hpp>

int main() {
    std::vector<uint8_t> trits = {1, 0, 1, 2, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 2, 1, 2, 0, 1};
    succinctrits::trit_vector tv(trits.begin(), trits.size());

    std::cout << "=== Trie Array ===" << std::endl;
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

    std::cout << "=== Operations ===" << std::endl;
    std::cout << "rank_0(6)   = " << tv_rs_0.rank(6) << std::endl;
    std::cout << "rank_1(10)  = " << tv_rs_1.rank(10) << std::endl;
    std::cout << "rank_2(3)   = " << tv_rs_2.rank(3) << std::endl;
    std::cout << "select_0(3) = " << tv_rs_0.select(3) << std::endl;
    std::cout << "select_1(5) = " << tv_rs_1.select(5) << std::endl;
    std::cout << "select_2(0) = " << tv_rs_2.select(0) << std::endl;

    std::cout << "=== Statistics ===" << std::endl;
    std::cout << "num trits: " << tv.get_num_trits() << std::endl;
    std::cout << "num 0s:    " << tv_rs_0.get_num_target_trits() << std::endl;
    std::cout << "num 1s:    " << tv_rs_1.get_num_target_trits() << std::endl;
    std::cout << "num 2s:    " << tv_rs_2.get_num_target_trits() << std::endl;

    {
        std::ofstream ofs("trits.idx");
        tv.save(ofs);
        tv_rs_0.save(ofs);
        tv_rs_1.save(ofs);
        tv_rs_2.save(ofs);
    }

    {
        std::ifstream ifs("trits.idx");
        succinctrits::trit_vector other_tv;
        succinctrits::rs_support<0> other_tv_rs_0;
        succinctrits::rs_support<1> other_tv_rs_1;
        succinctrits::rs_support<2> other_tv_rs_2;
        other_tv.load(ifs);
        other_tv_rs_0.load(ifs);
        other_tv_rs_1.load(ifs);
        other_tv_rs_2.load(ifs);
        other_tv_rs_0.set_vector(&other_tv);
        other_tv_rs_1.set_vector(&other_tv);
        other_tv_rs_2.set_vector(&other_tv);
    }

    std::remove("trits.idx");
    return 0;
}