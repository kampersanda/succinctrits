# succinctrits
This library provides succinct Rank/Select data structures on trits (i.e., ternary digits). An array consisting of n values from {0,1,2} is stored in 1.6n bits of space, which is close to the theoretically-optimal space usage of 1.57n bits [1]. The Rank/Select data structures are implemented in 0.32n additional bits of space in a similar manner to traditional approaches for bit arrays (e.g., [2]).

## Install

This library consists of only header files. Please pass the path to the directory [`succinctrits/include`](https://github.com/kampersanda/succinctrits/tree/master/include).

## Sample usage

```c++
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
```

The output will be

```
addr: 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 
trit: 1 0 1 2 1 0 0 1 0 1 0 0 1 1 1 2 1 2 0 1 
Rank_0(6)   = 2
Rank_1(10)  = 5
Rank_2(12)  = 1
Select_0(3) = 8
Select_1(5) = 12
Select_2(0) = 3
```

## References

1. Mihai Patrascu. Succincter. In FOCS, pages 305–313, 2008.
2. Rodrigo Gonz ́alez, Szymon Grabowski, Veli Ma ̈kinen, and Gonzalo Navarro. Practical implementation of rank and select queries. In WEA, pages 27–38, 2005.