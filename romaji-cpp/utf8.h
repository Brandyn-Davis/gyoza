#ifndef _CORE_UTF_H_
#define _CORE_UTF_H_

#include <stdint.h>
#include <stdexcept>

namespace core {

/* An integral type suitable for encoding a Unicode character. */
typedef uint_least32_t unicode_char;

/* Read the next UTF-8 character encoded in the null-terminated string input
and return the address of the next byte after it. */
const char *next_utf8_char(const char *input);

/* Read the next UTF-8 character from input as in next_utf8_char, and write the
encoded value to out. Return the address of the following byte. */
const char *read_utf8_char(const char *input, unicode_char &out);

} // namespace core

#endif

