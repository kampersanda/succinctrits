#pragma once

#include <cassert>
#include <vector>

namespace succinctrits {

class trit_vector_builder {
  public:
    trit_vector_builder() = default;
    ~trit_vector_builder() = default;

    void push_back(uint8_t t) {
        assert(t < 3);
        const size_t div = m_num_trits / 4;
        const size_t mod = m_num_trits % 4;
        if (mod == 0) {
            m_trytes.emplace_back(t);
        } else {
            m_trytes[div] |= (t << (mod * 2));
        }
        ++m_num_trits;
    }

    uint8_t get(size_t i) const {
        assert(i < m_num_trits);
        const size_t div = i / 4;
        const size_t mod = i % 4;
        return (m_trytes[div] >> (mod * 2)) % 4;
    }
    uint8_t operator[](size_t i) const {
        return get(i);
    }

    size_t get_num_trits() const {
        return m_num_trits;
    }

  private:
    std::vector<uint8_t> m_trytes;  // each of 2 bits
    size_t m_num_trits = 0;
};

}  // namespace succinctrits