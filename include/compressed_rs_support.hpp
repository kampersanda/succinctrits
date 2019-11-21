#pragma once

#include <array>
#include <algorithm>

#include "bit_vector.hpp"
#include "rs_support_bv.hpp"
#include "trit_vector.hpp"

namespace succinctrits {

class compressed_rs_support {
  public:
    using value_type = uint8_t;

    static constexpr size_t BITS_PER_WORD = bit_vector::BITS_PER_WORD;

  private:
    bit_vector bv_;
    rs_support_bv rs_bv_;
    std::array<value_type, 3> code_to_val_ = {0,1,2};
    std::array<value_type, 3> val_to_code_ = {0,1,2};
    uint64_t size_ = 0;
    uint64_t rank_size_ = 0;

    class trit_iterator :
            public std::iterator<std::random_access_iterator_tag, value_type, ptrdiff_t>
    {
      private:
        const compressed_rs_support* crs_;
        difference_type pos_;

      public:
        trit_iterator(const compressed_rs_support* crs, difference_type pos) : crs_(crs), pos_(pos) {}

        value_type operator*() const { return (*crs_)[pos_]; }

        trit_iterator& operator++() {
            ++pos_;
            return *this;
        }
        trit_iterator operator++(int) {
            trit_iterator iter = *this;
            ++pos_;
            return iter;
        }
        trit_iterator& operator--() {
            --pos_;
            return *this;
        }
        trit_iterator operator--(int) {
            trit_iterator iter = *this;
            --pos_;
            return iter;
        }

        trit_iterator operator+(difference_type diff) const { return {crs_, pos_ + diff}; }
        friend trit_iterator operator+(difference_type diff, const trit_iterator& iter) { return iter + diff; }
        trit_iterator operator-(difference_type diff) const { return *this + -diff; }
        difference_type operator-(const trit_iterator& iter) const { return pos_ - iter.pos_; }

        bool operator<(const trit_iterator& iter) const {
            assert(crs_ == iter.crs_);
            return pos_ < iter.pos_;
        }
        bool operator==(const trit_iterator& iter) const { return crs_ == iter.crs_ and pos_ == iter.pos_; }
	  	bool operator!=(const trit_iterator& iter) const { return !(*this == iter); }
        bool operator<=(const trit_iterator& iter) const { return pos_ <= iter.pos_; }
        bool operator>(const trit_iterator& iter) const { return !(iter <= *this); }
        bool operator>=(const trit_iterator& iter) const { return !(iter < *this); }

        trit_iterator& operator+=(difference_type diff) {
            pos_ += diff;
            return *this;
        }
        trit_iterator& operator-=(difference_type diff) {
            pos_ -= diff;
            return *this;
        }

        value_type operator[](difference_type diff) { return *(*this + diff); }

    };

  public:
  	using const_iterator = trit_iterator;
  	using iterator = const_iterator;
    using const_pointer = const_iterator;
  	using pointer = const_pointer;

    compressed_rs_support() = default;

    explicit compressed_rs_support(const trit_vector& vec) {
        size_t val_cnt[] = {0, 0, 0};
        for (size_t i = 0; i < vec.size(); i++) {
            val_cnt[vec[i]]++;
        }
        code_to_val_ = {0,1,2};
        std::sort(code_to_val_.begin(), code_to_val_.end(),
                  [&](auto l, auto r) { return val_cnt[l] > val_cnt[r]; });
        for (int i = 0; i < 3; i++) {
            val_to_code_[code_to_val_[i]] = i;
        }

        bv_.resize(vec.size());
        for (size_t i = 0; i < vec.size(); i++) {
            auto code = val_to_code_[vec[i]];
            bv_[i] = code != 0;
            if (code != 0) {
                bv_.push_back(code == 2);
            }
        }

        rs_bv_.build(&bv_);
        size_ = vec.size();
        rank_size_ = rs_bv_.rank(vec.size());
    }

    void build(const trit_vector& vec) {
        *this = compressed_rs_support(vec);
    }

    value_type operator[](size_t i) const {
        if (rs_bv_[i] == 0) {
            return code_to_val_[0];
        } else {
            return code_to_val_[1 + rs_bv_[size_ + rs_bv_.rank(i)]];
        }
    }
    value_type get(size_t i) const {
        return operator[](i);
    }

    // Returns the number of occurrences of the target trits in "this" between positions 0 and i-1.
    template <value_type Trit>
    size_t rank(size_t i) const {
        static_assert(Trit < 3, "Template value is invalid.");
        if (val_to_code_[Trit] == 0) {
            return rs_bv_.rank0(i);
        } else {
            if (val_to_code_[Trit] == 1) {
                return rs_bv_.rank0(size_+rs_bv_.rank(i)) - (size_-rank_size_);
            } else { // code is 2
                return rs_bv_.rank(size_+rs_bv_.rank(i)) - rank_size_;
            }
        }
    }

    // Returns the position of the (n+1)-th occurrence of the target trit in "this".
    template <value_type Trit>
    size_t select(size_t n) const {
        static_assert(Trit < 3, "Template value is invalid.");
        if (val_to_code_[Trit] == 0) {
            return rs_bv_.select0(n);
        } else {
            if (val_to_code_[Trit] == 1) {
                return rs_bv_.select(rs_bv_.select0(size_-rank_size_+n) - size_);
            } else { // code is 2
                return rs_bv_.select(rs_bv_.select(rank_size_+n) - size_);
            }
        }
    }

	iterator begin() { return {this, 0}; }

	iterator end() { return {this, iterator::difference_type(size_)}; }

    const_iterator begin() const { return {this, 0}; }

  	const_iterator end() const { return {this, const_iterator::difference_type(size_)}; }

    size_t size() const { return size_; }

    size_t get_num_trits() const { return size(); }

    size_t size_in_bytes() const {
        return (bv_.size_in_bytes() +
                rs_bv_.size_in_bytes() +
                sizeof(uint8_t) * 3 * 2 +
                sizeof(size_t) * 2);
    }

    void save(std::ostream& os) const {
        bv_.save(os);
        rs_bv_.save(os);
        os.write(reinterpret_cast<const char*>(code_to_val_.data()), sizeof(uint8_t) * 3);
        os.write(reinterpret_cast<const char*>(val_to_code_.data()), sizeof(uint8_t) * 3);
        os.write(reinterpret_cast<const char*>(&size_), sizeof(uint64_t));
        os.write(reinterpret_cast<const char*>(&rank_size_), sizeof(uint64_t));
    }

    void load(std::istream& is) {
        bv_.load(is);
        rs_bv_.load(is);
        rs_bv_.set_bv(&bv_);
        is.read(reinterpret_cast<char*>(code_to_val_.data()), sizeof(uint8_t) * 3);
        is.read(reinterpret_cast<char*>(val_to_code_.data()), sizeof(uint8_t) * 3);
        is.read(reinterpret_cast<char*>(&size_), sizeof(uint64_t));
        is.read(reinterpret_cast<char*>(&rank_size_), sizeof(uint64_t));
    }

};

}
