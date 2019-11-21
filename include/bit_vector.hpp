#pragma once

#include <vector>
#include <iostream>
#include <cassert>

namespace succinctrits {

class bit_vector {
  public:
    using storage_type = uint64_t;
    using storage_pointer = storage_type *;
    using const_storage_pointer = const storage_type *;

    static constexpr size_t BITS_PER_WORD = 64;

  private:
    std::vector<storage_type> storage_;
    uint64_t size_ = 0;

    class bit_reference {
      private:
        storage_pointer pointer_;
        storage_type mask_;

      public:
        bit_reference(storage_pointer pointer, storage_type mask) : pointer_(pointer), mask_(mask) {}

        bit_reference& operator=(bool x) {
            if (x)
                *pointer_ |= mask_;
            else
                *pointer_ &= ~mask_;
            return *this;
        }

        bit_reference& operator=(const bit_reference& x) {
            if (*(x.pointer_) & x.mask_)
                *pointer_ |= mask_;
            else
                *pointer_ &= ~mask_;
            return *this;
        }

        bool operator~() const {
            return (*pointer_ & mask_) == 0;
        }

        operator bool() const {
            return (*pointer_ & mask_) != 0;
        }

        bit_reference& flip() {
            *pointer_ ^= mask_;
            return *this;
        }
    };

    storage_type _word(size_t index) const {
        return storage_[index / BITS_PER_WORD];
    }

    storage_pointer _word_ptr(size_t index) {
        return storage_.data() + index / BITS_PER_WORD;
    }

    auto _word_iter(size_t index) {
        return storage_.begin() + (index / BITS_PER_WORD);
    }

    constexpr storage_type _mask(size_t ctz) const {
        assert(ctz < BITS_PER_WORD);
        return 1ull << ctz;
    }

  public:
    bit_vector() = default;

    explicit bit_vector(size_t size) {
        resize(size);
    }

    bit_reference operator[](size_t index) {
        return {storage_.data() + index / BITS_PER_WORD,
                _mask(index % BITS_PER_WORD)};
    }

    bool operator[](size_t index) const {
        return (_word(index) & _mask(index % BITS_PER_WORD)) != 0;
    }

    bool get(size_t index) const { return operator[](index); }

    size_t size() const { return size_; }

    void resize(size_t new_size) {
        size_ = new_size;
        storage_.resize((new_size-1) / BITS_PER_WORD + 1);
    }

    void clear() {
        resize(0);
        storage_.shrink_to_fit();
    }

    void push_back(bool x) {
        resize(size() + 1);
        operator[](size()-1) = x;
    }

    storage_pointer data() { return storage_.data(); }

    const_storage_pointer data() const { return storage_.data(); }

    size_t size_in_bytes() const {
        return (storage_.size() * sizeof(storage_type) + sizeof(size_t));
    }

    void save(std::ostream& os) const {
        size_t n = storage_.size();
        os.write(reinterpret_cast<const char*>(&n), sizeof(uint64_t));
        os.write(reinterpret_cast<const char*>(storage_.data()), n * sizeof(storage_type));
        os.write(reinterpret_cast<const char*>(&size_), sizeof(uint64_t));
    }
    void load(std::istream& is) {
        size_t n;
        is.read(reinterpret_cast<char*>(&n), sizeof(uint64_t));
        is.read(reinterpret_cast<char*>(storage_.data()), n * sizeof(storage_type));
        is.read(reinterpret_cast<char*>(&size_), sizeof(uint64_t));
    }

};

}
