#include <chrono>
#include <iostream>
#include <random>

#include <rank_support.hpp>
#include <trit_vector.hpp>

class timer {
  public:
    using hrc = std::chrono::high_resolution_clock;

    timer() = default;

    template <class Duration>
    double get() const {
        return std::chrono::duration_cast<Duration>(hrc::now() - tp_).count();
    }

  private:
    hrc::time_point tp_ = hrc::now();
};

std::vector<uint8_t> generate_trits(uint32_t num_trits) {
    std::random_device rd;
    std::vector<uint8_t> trits(num_trits);
    for (uint32_t i = 0; i < num_trits; ++i) {
        trits[i] = uint8_t(rd() % 3);
    }
    return trits;
}

std::vector<uint32_t> generate_indices(uint32_t num_trits, uint32_t num_queries) {
    std::random_device rd;
    std::vector<uint32_t> indices(num_queries);
    for (uint32_t i = 0; i < num_queries; ++i) {
        indices[i] = uint8_t(rd() % num_trits);
    }
    return indices;
}

template <uint8_t Trit>
void rank_benchmark(const succinctrits::trit_vector& tv, const std::vector<uint32_t>& indices) {
    std::cout << "=== benchmar<" << int(Trit) << ">: "  //
              << tv.get_num_trits() << " trits; " << indices.size() << " queries ===" << std::endl;
    succinctrits::rank_support<Trit> tv_r(&tv);
    timer t;
    for (uint32_t i = 0; i < indices.size(); ++i) {
        uint32_t rank = tv_r.rank(indices[i]);
        if (rank >= tv.get_num_trits()) {
            std::cerr << "CRITICAL ERROR" << std::endl;
            return;
        }
    }
    const double elapsed_nanosec = t.get<std::chrono::nanoseconds>();
    const double tv_size_in_bits = tv.size_in_bytes() * 8.0;
    const double rank_size_in_bits = tv_r.size_in_bytes() * 8.0;

    std::cout << "# runtime:\t" << elapsed_nanosec / indices.size() << " ns/op" << std::endl;
    std::cout << "# trit_vector:\t" << tv_size_in_bits / tv.get_num_trits() << " bits/trit" << std::endl;
    std::cout << "# rank_support:\t" << rank_size_in_bits / tv.get_num_trits() << " bits/trit" << std::endl;
}

int main() {
    std::vector<uint32_t> nums_trits = {100'000, 1'000'000, 10'000'000};
    for (uint32_t num_trits : nums_trits) {
        auto trits = generate_trits(num_trits);
        auto indices = generate_indices(num_trits, 10'000);
        succinctrits::trit_vector tv(trits.begin(), trits.size());
        rank_benchmark<0>(tv, indices);
        rank_benchmark<1>(tv, indices);
        rank_benchmark<2>(tv, indices);
    }
}