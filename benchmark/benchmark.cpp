#include <chrono>
#include <iostream>
#include <random>

#include <rs_support.hpp>
#include <trit_vector.hpp>

static constexpr uint64_t NUM_QUERIES = 100'000;

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

std::vector<uint8_t> generate_trits(uint64_t num_trits) {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint8_t> dist(0, 2);

    std::vector<uint8_t> trits(num_trits);
    for (uint64_t i = 0; i < num_trits; ++i) {
        trits[i] = dist(engine);
    }
    return trits;
}

void benchmark_access(const succinctrits::trit_vector& tv) {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint64_t> dist(0, tv.get_num_trits() - 1);

    timer t;
    for (uint64_t i = 0; i < NUM_QUERIES; ++i) {
        uint8_t trit = tv[dist(engine)];
        if (3 <= trit) {  // to avoid opt.
            std::cerr << "critical error" << std::endl;
            exit(1);
        }
    }
    const double elapsed_nanosec = t.get<std::chrono::nanoseconds>();
    std::cout << "# access time: " << elapsed_nanosec / NUM_QUERIES << " ns/op" << std::endl;
}

template <uint8_t Trit>
void benchmark_rank(const succinctrits::rs_support<Trit>& tv_rs) {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint64_t> dist(0, tv_rs.get_num_trits() - 1);

    timer t;
    for (uint64_t i = 0; i < NUM_QUERIES; ++i) {
        uint64_t rank = tv_rs.rank(dist(engine));
        if (tv_rs.get_num_target_trits() < rank) {  // to avoid opt.
            std::cerr << "critical error" << std::endl;
            exit(1);
        }
    }
    const double elapsed_nanosec = t.get<std::chrono::nanoseconds>();
    std::cout << "# rank time:   " << elapsed_nanosec / NUM_QUERIES << " ns/op" << std::endl;
}

template <uint8_t Trit>
void benchmark_select(const succinctrits::rs_support<Trit>& tv_rs) {
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint64_t> dist(0, tv_rs.get_num_target_trits() - 1);

    timer t;
    for (uint64_t i = 0; i < NUM_QUERIES; ++i) {
        uint64_t select = tv_rs.select(dist(engine));
        if (tv_rs.get_num_trits() <= select) {  // to avoid opt.
            std::cerr << "critical error" << std::endl;
            exit(1);
        }
    }
    const double elapsed_nanosec = t.get<std::chrono::nanoseconds>();
    std::cout << "# select time: " << elapsed_nanosec / NUM_QUERIES << " ns/op" << std::endl;
}

int main() {
    std::vector<uint32_t> nums_trits = {1'000'000, 10'000'000, 100'000'000};

    for (uint32_t num_trits : nums_trits) {
        std::cout << "=== Benchmark for " << num_trits << " trits ===" << std::endl;

        auto trits = generate_trits(num_trits);
        succinctrits::trit_vector tv(trits.begin(), trits.size());
        succinctrits::rs_support<0> tv_rs(&tv);

        benchmark_access(tv);
        benchmark_rank(tv_rs);
        benchmark_select(tv_rs);

        const double tv_size_in_bits = tv.size_in_bytes() * 8.0;
        const double rs_size_in_bits = tv_rs.size_in_bytes() * 8.0;
        std::cout << "# trit_vector: " << tv_size_in_bits / tv.get_num_trits() << " bits/trit" << std::endl;
        std::cout << "# rs_support:  " << rs_size_in_bits / tv.get_num_trits() << " bits/trit" << std::endl;
    }
}