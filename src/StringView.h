/**
 * @file StringView.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief String view library
 * Simple single-header string-view implementation
 *
 * @date 2025-02-22
 * @copyright (C) 2025 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2025 WolodiaM
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __CBUILD_SV_H__
#define __CBUILD_SV_H__
// Code
typedef struct cbuild_sv_t {
  char *data;
  size_t size;
} cbuild_sv_t;
// // Code
// #define SVFmt "%.*s"
// #define SVArg(sv) (int)(sv).size, (sv).data
// /**
//  * @brief Create string view
//  *
//  * @param data => const char* -> Char array
//  * @param size => size_t -> Length of a view
//  * @return sv_t -> New string view
//  */
// sv_t sv_from_parts(const char *data, size_t size);
// /**
//  * @brief Create string view from c string
//  *
//  * @param cstr => const char* -> C-string
//  * @return sv_t -> New string view
//  */
// sv_t sv_from_cstr(const char *cstr);
// /**
//  * @brief Trim whitespaces from a string view starting from the left
//  *
//  * @param sv => sv_t* -> String view to work with
//  * @return size_t -> Number of characters removed
//  */
// size_t sv_trim_left(sv_t *sv);
// /**
//  * @brief Trim whitespaces from a string view starting from the right
//  *
//  * @param sv => sv_t* -> String view to work with
//  * @return size_t -> Number of characters removed
//  */
// size_t sv_trim_right(sv_t *sv);
// /**
//  * @brief Trim whitespaces from a string view starting from both sides
//  *
//  * @param sv => sv_t* -> String view to work with
//  * @return size_t -> Number of characters removed
//  */
// size_t sv_trim(sv_t *sv);
// /**
//  * @brief Chop characters from one string view into another
//  * Note: Original string view will be truncated
//  * @param sv => sv_t* -> String view to work with
//  * @param size => size_t -> Number of characters to chop
//  * @return sv_t -> New string view
//  */
// sv_t sv_chop(sv_t *sv, size_t size);
// /**
//  * @brief Chop characters from one string view into another
//  * Note: Original string view will be truncated. delim will be removed from both
//  * string views
//  * @param sv => sv_t* -> String view to work with
//  * @param delim => char -> Character on which function stops.
//  * @return sv_t -> New string view
//  */
// sv_t sv_chop_by_delim(sv_t *sv, char delim);
// /**
//  * @brief Chop characters from one string view into another
//  * Note: Original string view will be truncated. delim will be removed from both
//  * string views
//  * @param sv => sv_t* -> String view to work with
//  * @param delim => sv_t -> Character on which function stops.
//  * @return sv_t -> New string view
//  */
// sv_t sv_chop_by_sv(sv_t *sv, sv_t delim);
// /**
//  * @brief Delimiter func for sv_chop_by_delim
//  *
//  * @param sv => const sv_t* -> String view currently being chopped
//  * @param idx => size_t -> Current scan index
//  * @param args => void* -> Some user arguments
//  * @return bool -> True if this character should be marked as a delimiter
//  */
// typedef bool (*sv_delim_func)(const sv_t *sv, size_t idx, void *args);
// /**
//  * @brief Chop characters from one string view into another
//  * Note: Original string view will be truncated. Index which function marked as
//  * an end will not be in any string view string views.
//  * @param sv => sv_t* -> String view to work with
//  * @param delim => sv_delim_func -> Delimiter scanner
//  * @param args => void* -> Will be passed into delim function
//  * @return sv_t -> New string view
//  */
// sv_t sv_chop_by_func(sv_t *sv, sv_delim_func delim, void *args);
// /**
//  * @brief strcmp for string view
//  *
//  * @param a => sv_t -> First StringView
//  * @param b => sv_t -> Second StringView
//  * @return -2 -> If size of first string view is smaller
//  * @return -1 -> If first different character in first string view is smaller
//  * @return 0  -> If two string views are equal
//  * @return 1  -> If first different character in first string view is larger
//  * @return 2  -> If size of first string view is larger
//  */
// int sv_cmp(sv_t a, sv_t b);
// /**
//  * @brief compare two string view ignoring case of a latin letters for string
//  * view
//  *
//  * @param a => sv_t -> First StringView
//  * @param b => sv_t -> Second StringViewr
//  * @return -2 -> If size of first string view is smaller
//  * @return -1 -> If first different character in first string view is smaller
//  * @return 0  -> If two string views are equal
//  * @return 1  -> If first different character in first string view is larger
//  * @return 2  -> If size of first string view is larger
//  */
// int sv_cmp_icase(sv_t a, sv_t b);
// /**
//  * @brief Check if string string view has specific prefix
//  *
//  * @param sv => sv_t -> String view to work with
//  * @param prefix => sv_t -> Prefix
//  * @return bool -> True if sv contains prefix
//  */
// bool sv_prefix(sv_t sv, sv_t prefix);
// /**
//  * @brief Check if string string view has specific suffix
//  *
//  * @param sv => sv_t -> String view to work with
//  * @param suffix => sv_t -> Prefix
//  * @return bool -> True if sv contains suffix
//  */
// bool sv_suffix(sv_t sv, sv_t suffix);
// // Code
// #ifdef SV_IMPLEMENTATION
// sv_t sv_from_parts(const char *data, size_t size) {
//   return (sv_t){.data = (char *)data, .size = size};
// }
// sv_t sv_from_cstr(const char *cstr) {
//   return (sv_t){.data = (char *)cstr, .size = strlen(cstr)};
// }
// size_t sv_trim_left(sv_t *sv) {
//   size_t i = 0;
//   while (i < sv->size && isspace(sv->data[i])) {
//     i++;
//   }
//   sv->data += i;
//   sv->size -= i;
//   return i;
// }
// size_t sv_trim_right(sv_t *sv) {
//   size_t i = sv->size;
//   while (i > 0 && isspace(sv->data[i - 1])) {
//     i--;
//   }
//   int tmp = sv->size - i;
//   sv->size -= tmp;
//   return tmp;
// }
// size_t sv_trim(sv_t *sv) {
//   size_t ret = sv_trim_left(sv);
//   ret += sv_trim_right(sv);
//   return ret;
// }
// sv_t sv_chop(sv_t *sv, size_t size) {
//   if (size > sv->size) {
//     size = sv->size;
//   }
//   char *tmp = sv->data;
//   sv->data += size;
//   sv->size -= size;
//   return sv_from_parts(tmp, size);
// }
// bool sv___chop_by_delim_delim(const sv_t *sv, size_t idx, void *args) {
//   return *((char *)args) == sv->data[idx];
// }
// sv_t sv_chop_by_delim(sv_t *sv, char delim) {
//   return sv_chop_by_func(sv, sv___chop_by_delim_delim, &delim);
// }
// bool sv___chop_by_sv_delim1(const sv_t *sv, size_t idx, void *args) {
//   sv_t *svt = args;
//   size_t size = idx < svt->size ? idx : svt->size;
//   return sv_cmp(sv_from_parts(sv->data + idx - (size - 1), size), *svt) == 0;
// }
// bool sv___chop_by_sv_delim2(const sv_t *sv, size_t idx, void *args) {
//   sv_t *svt = args;
//   size_t size = (svt->size - 1) + idx > sv->size ? 0 : svt->size - 1;
//   return sv_cmp(sv_from_parts(sv->data + idx, size),
//                 sv_from_parts(svt->data, svt->size - 1)) == 0;
// }
// sv_t sv_chop_by_sv(sv_t *sv, sv_t delim) {
//   sv_t tmp = sv_chop_by_func(sv, sv___chop_by_sv_delim1, &delim);
//   return sv_chop_by_func(&tmp, sv___chop_by_sv_delim2, &delim);
// }
// sv_t sv_chop_by_func(sv_t *sv, sv_delim_func delim, void *args) {
//   size_t i = 0;
//   while (i <= sv->size && !delim(sv, i, args)) {
//     i++;
//   }
//   if (i >= sv->size) {
//     return sv_chop(sv, i);
//   }
//   char *tmp = sv->data;
//   sv->data += i + 1;
//   sv->size -= i + 1;
//   return sv_from_parts(tmp, i);
// }
// int sv_cmp(sv_t a, sv_t b) {
//   if (a.size < b.size) {
//     return -2;
//   }
//   if (a.size > b.size) {
//     return 2;
//   }
//   int ret = memcmp(a.data, b.data, a.size);
//   if (ret == 0) {
//     return 0;
//   } else if (ret < 0) {
//     return -1;
//   } else if (ret > 0) {
//     return 1;
//   }
//   return 0;
// }
// int sv_cmp_icase(sv_t a, sv_t b) {
//   if (a.size < b.size) {
//     return -2;
//   }
//   if (a.size > b.size) {
//     return 2;
//   }
//   for (size_t i = 0; i < a.size; i++) {
//     char ac = 'A' <= a.data[i] && a.data[i] <= 'Z' ? a.data[i] + 32 : a.data[i];
//     char bc = 'A' <= b.data[i] && b.data[i] <= 'Z' ? b.data[i] + 32 : b.data[i];
//     int diff = ac - bc;
//     if (diff < 0) {
//       return -1;
//     } else if (diff > 0) {
//       return 1;
//     }
//   }
//   return 0;
// }
// bool sv_prefix(sv_t sv, sv_t prefix) {
//   if (sv.size < prefix.size) {
//     return false;
//   }
//   return sv_cmp(sv_from_parts(sv.data, prefix.size), prefix) == 0;
// }
// bool sv_suffix(sv_t sv, sv_t suffix) {
//   if (sv.size < suffix.size) {
//     return false;
//   }
//   return sv_cmp(sv_from_parts(sv.data + sv.size - suffix.size, suffix.size),
//                 suffix) == 0;
// }
// #endif // SV_IMPLEMENTATION
#endif // __CBUILD_SV_H_
