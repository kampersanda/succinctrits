#pragma once

#include "bit_vector.hpp"
#include "bo/popcnt.hpp"
#include "bo/select.hpp"

namespace succinctrits {

class rs_support_bv {
  public:
    static constexpr size_t L_BLOCK_SIZE = 512;
    static constexpr size_t S_BLOCK_SIZE = 64;
    static constexpr size_t NUM_S_PER_L = L_BLOCK_SIZE / S_BLOCK_SIZE; // 8

    using storage_type = bit_vector::storage_type;

  private:
    const bit_vector* bv_ = nullptr;
    std::vector<uint64_t> basic_block_;
    std::vector<uint64_t> select_tips_;
    std::vector<uint64_t> select0_tips_;

    storage_type _word(size_t w) const {
        return *(bv_->data() + w);
    }

    size_t _l_block(size_t l) const {
        return basic_block_[l * 2];
    }

    size_t _s_block(size_t l, size_t s) const {
        assert(s < 8);
        return (basic_block_[l * 2 + 1] >> (9*(7-s))) & 0x1FFu;
    }

  public:
    rs_support_bv() = default;

    explicit rs_support_bv(const bit_vector* bv) : bv_(bv) {
        const size_t num_lb = (bv->size()-1) / L_BLOCK_SIZE + 1;
        const size_t num_sb = (bv->size()-1) / S_BLOCK_SIZE + 1;
        basic_block_.resize((num_sb / NUM_S_PER_L + 1) * 2);
        select_tips_.push_back(0);
        select0_tips_.push_back(0);
        size_t cnt_all = 0;
        size_t cnt_mod_l = 0;
        size_t cnt0_mod_l = 0;
        for (size_t l = 0; l < num_lb; l++) {
            basic_block_[l*2] = cnt_all;
            size_t cnt_l = 0;
            for (size_t s = 0; s < NUM_S_PER_L; s++) {
                assert(cnt_l < 512);
                basic_block_[l*2+1] |= cnt_l << (9*(7-s));
                if (l*NUM_S_PER_L+s >= num_sb) {
                    for (auto ss = s+1; ss < NUM_S_PER_L; ss++)
                        basic_block_[l*2+1] |= cnt_l << (9*(7-ss));
                    break;
                }
                cnt_l += bo::popcnt_u64(_word(l * NUM_S_PER_L + s));
            }
            cnt_all += cnt_l;
            cnt_mod_l += cnt_l;
            cnt0_mod_l += L_BLOCK_SIZE - cnt_l;
            if (cnt_mod_l >= L_BLOCK_SIZE) {
                select_tips_.push_back(l);
                cnt_mod_l %= L_BLOCK_SIZE;
            }
            if (cnt0_mod_l >= L_BLOCK_SIZE) {
                select0_tips_.push_back(l);
                cnt0_mod_l %= L_BLOCK_SIZE;
            }
        }
        if (num_sb % NUM_S_PER_L == 0) {
            basic_block_[num_lb*2] = cnt_all;
        }
        select_tips_.push_back(basic_block_.size()/2-1);
    }

    void build(const bit_vector* bv) { *this = rs_support_bv(bv); }

    void set_bv(const bit_vector* bv) { bv_ = bv; }

    bool operator[](size_t index) const { return bool(bv_->get(index)); }

    bool get(size_t index) const { return operator[](index); }

    size_t rank(size_t index) const {
        assert(bv_ != nullptr);
        return (_l_block(index / L_BLOCK_SIZE) +
                _s_block(index / L_BLOCK_SIZE, index / S_BLOCK_SIZE % NUM_S_PER_L)
                + bo::popcnt_u64(_word(index / S_BLOCK_SIZE) % (1ull<<(index%S_BLOCK_SIZE))));
    }

    size_t rank0(size_t index) const { return index - rank(index); }

    size_t select(size_t n) const {
        assert(bv_ != nullptr);
        auto left = select_tips_[n / L_BLOCK_SIZE];
        auto right = select_tips_[n / L_BLOCK_SIZE + 1] + 1;
        auto l = left;
        while (left < right) {
            auto c = (left + right) / 2;
            if (_l_block(c) <= n) {
                l = c;
                left = c + 1;
            } else {
                right = c;
            }
        }

        left = 0;
        right = NUM_S_PER_L;
        auto n_sub_l = n - _l_block(l);
        auto s = left;
        while (left < right) {
            auto c = (left + right) / 2;
            if (_s_block(l, c) <= n_sub_l) {
                s = c;
                left = c + 1;
            } else {
                right = c;
            }
        }

        auto n_sub_s = n_sub_l - _s_block(l, s);
        return (l * L_BLOCK_SIZE +
                s * S_BLOCK_SIZE +
                bo::select_u64(_word(l * NUM_S_PER_L + s), n_sub_s));
    }

    size_t select0(size_t n) const {
        assert(bv_ != nullptr);
        auto left = select0_tips_[n / L_BLOCK_SIZE];
        auto right = select0_tips_[n / L_BLOCK_SIZE + 1] + 1;
        auto l = left;
        while (left < right) {
            auto c = (left + right) / 2;
            if ((c*L_BLOCK_SIZE-_l_block(c)) <= n) {
                l = c;
                left = c + 1;
            } else {
                right = c;
            }
        }

        left = 0;
        right = NUM_S_PER_L;
        auto n_sub_l = n - (l*L_BLOCK_SIZE-_l_block(l));
        auto s = left;
        while (left < right) {
            auto c = (left + right) / 2;
            if ((c*S_BLOCK_SIZE-_s_block(l, c)) <= n_sub_l) {
                s = c;
                left = c + 1;
            } else {
                right = c;
            }
        }

        auto n_sub_s = n_sub_l - (s*S_BLOCK_SIZE-_s_block(l, s));
        return (l * L_BLOCK_SIZE +
                s * S_BLOCK_SIZE +
                bo::select_u64(~_word(l * NUM_S_PER_L + s), n_sub_s));
    }

    size_t size() const { return bv_->size(); }

    size_t size_in_bytes() const {
        return (basic_block_.size() * sizeof(uint64_t) +
                select_tips_.size() * sizeof(uint64_t) +
                select0_tips_.size() * sizeof(uint64_t));
    }

    void save(std::ostream& os) const {
        size_t n_b = basic_block_.size();
        os.write(reinterpret_cast<const char*>(&n_b), sizeof(uint64_t));
        os.write(reinterpret_cast<const char*>(basic_block_.data()), n_b * sizeof(uint64_t));
        size_t n_s = select_tips_.size();
        os.write(reinterpret_cast<const char*>(&n_s), sizeof(uint64_t));
        os.write(reinterpret_cast<const char*>(select_tips_.data()), n_b * sizeof(uint64_t));
        size_t n_s0 = select0_tips_.size();
        os.write(reinterpret_cast<const char*>(&n_s0), sizeof(uint64_t));
        os.write(reinterpret_cast<const char*>(select0_tips_.data()), n_b * sizeof(uint64_t));
    }

    void load(std::istream& is) {
        size_t n_b;
        is.read(reinterpret_cast<char*>(&n_b), sizeof(uint64_t));
        is.read(reinterpret_cast<char*>(basic_block_.data()), n_b * sizeof(uint64_t));
        size_t n_s;
        is.read(reinterpret_cast<char*>(&n_s), sizeof(uint64_t));
        is.read(reinterpret_cast<char*>(select_tips_.data()), n_b * sizeof(uint64_t));
        size_t n_s0;
        is.read(reinterpret_cast<char*>(&n_s0), sizeof(uint64_t));
        is.read(reinterpret_cast<char*>(select0_tips_.data()), n_b * sizeof(uint64_t));
    }

};

}
