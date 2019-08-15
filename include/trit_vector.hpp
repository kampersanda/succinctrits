#pragma once

#include <cassert>
#include <vector>

#include "trit_vector_builder.hpp"

namespace succinctrits {

template <uint8_t>
class rank_support;

class trit_vector {
  public:
    trit_vector() = default;

    trit_vector(const trit_vector_builder& b) {
        build(b);
    }

    ~trit_vector() = default;

    void build(const trit_vector_builder& b) {
        m_num_trits = b.get_num_trits();
        m_trytes.clear();
        m_trytes.resize(m_num_trits / TRITS_PER_BYTE + 1);

        m_trit_counts[0] = 0;
        m_trit_counts[1] = 0;
        m_trit_counts[2] = 0;

        uint8_t tryte[TRITS_PER_BYTE];
        for (size_t i = 0; i < m_num_trits; i += TRITS_PER_BYTE) {
            for (size_t j = 0; j < TRITS_PER_BYTE; ++j) {
                if (i + j < m_num_trits) {
                    tryte[j] = b.get(i + j);
                    ++m_trit_counts[tryte[j]];
                } else {
                    tryte[j] = 0;
                }
            }
            m_trytes[i / TRITS_PER_BYTE] = to_byte(tryte);
        }
    }

    uint8_t get(size_t i) const {
        assert(i < m_num_trits);

        const size_t div = i / TRITS_PER_BYTE;
        const size_t mod = i % TRITS_PER_BYTE;

        switch (mod) {
            case 0:
                return m_trytes[div] % 3;
            case 1:
                return m_trytes[div] / 3 % 3;
            case 2:
                return m_trytes[div] / 9 % 3;
            case 3:
                return m_trytes[div] / 27 % 3;
            case 4:
                return m_trytes[div] / 81 % 3;
            default:
                // should not come
                assert(false);
        }
    }
    uint8_t operator[](size_t i) const {
        return get(i);
    }

    size_t get_num_trits() const {
        return m_num_trits;
    }

    size_t get_num_0s() const {
        return m_trit_counts[0];
    }
    size_t get_num_1s() const {
        return m_trit_counts[1];
    }
    size_t get_num_2s() const {
        return m_trit_counts[2];
    }

  private:
    static constexpr size_t TRITS_PER_BYTE = 5;

    std::vector<uint8_t> m_trytes;  // each of 5 trits
    size_t m_num_trits = 0;
    size_t m_trit_counts[3];

    uint8_t to_byte(const uint8_t* tryte) {
        return tryte[0] + tryte[1] * 3 + tryte[2] * 9 + tryte[3] * 27 + tryte[4] * 81;
    }

    friend class rank_support<0>;
    friend class rank_support<1>;
    friend class rank_support<2>;
};

}  // namespace succinctrits