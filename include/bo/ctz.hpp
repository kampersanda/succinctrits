/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
 */

#ifndef BO_CTZ_HPP_
#define BO_CTZ_HPP_

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "popcnt.hpp"

namespace bo {

inline uint8_t ctz_u8(uint8_t x) {
  return popcnt_u8((x & -x) - 1);
}

inline uint16_t ctz_u16(uint16_t x) {
#ifdef __BMI__

  return _tzcnt_u16(x);

#else

  return popcnt_u16((x & -x) - 1);

#endif
}

inline uint32_t ctz_u32(uint32_t x) {
#ifdef __BMI__

  return _tzcnt_u32(x);

#else

  return popcnt_u32((x & -x) - 1);

#endif
}

inline uint64_t ctz_u64(uint64_t x) {
#ifdef __BMI__

  return _tzcnt_u64(x);

#else

  return popcnt_u64((x & -x) - 1);

#endif
}

}

#endif //BO_CTZ_HPP_
