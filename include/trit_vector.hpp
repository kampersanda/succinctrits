#pragma once

#include <cassert>
#include <vector>

namespace succinctrits {

template <uint8_t>
class rank_support;

class trit_vector {
  public:
    class builder {
      public:
        builder() = default;

        void reserve(uint32_t capa) {
            m_trytes.reserve(capa / 5U + 1U);
        }
        void push_back(uint8_t t) {
            assert(t < 3);

            ++m_count;
            ++m_num_trits;

            if (m_count == 1) {
                m_trytes.emplace_back(t);
            } else if (m_count == 2) {
                m_trytes.back() += t * 3U;
            } else if (m_count == 3) {
                m_trytes.back() += t * 9U;
            } else if (m_count == 4) {
                m_trytes.back() += t * 27U;
            } else if (m_count == 5) {
                m_trytes.back() += t * 81U;
                m_count = 0;
            }
        }

      private:
        std::vector<uint8_t> m_trytes;
        uint32_t m_num_trits = 0;
        uint32_t m_count = 0;

        friend class trit_vector;
    };

  public:
    trit_vector() = default;

    template <class Iterator>
    trit_vector(Iterator it, uint32_t num_trits) {
        build(it, num_trits);
    }
    explicit trit_vector(builder* b) {
        build(b);
    }

    template <class Iterator>
    void build(Iterator it, uint32_t num_trits) {
        builder b;
        b.reserve(num_trits);
        for (uint32_t i = 0; i < num_trits; ++i) {
            b.push_back(*it);
            ++it;
        }
        build(&b);
    }
    void build(builder* b) {
        m_trytes = std::move(b->m_trytes);
        m_num_trits = b->m_num_trits;
    }

    uint8_t get(uint32_t i) const {
        assert(i < m_num_trits);

        const uint32_t div = i / TRITS_PER_BYTE;
        const uint32_t mod = i % TRITS_PER_BYTE;

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
    uint8_t operator[](uint32_t i) const {
        return get(i);
    }

    uint32_t get_num_trits() const {
        return m_num_trits;
    }

  private:
    static constexpr uint32_t TRITS_PER_BYTE = 5;

    std::vector<uint8_t> m_trytes;  // each of 5 trits
    uint32_t m_num_trits = 0;

    friend class rank_support<0>;
    friend class rank_support<1>;
    friend class rank_support<2>;
};

}  // namespace succinctrits