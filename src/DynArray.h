/**
 * @file DynArray.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief DynArray implememetation
 * Implementation of dynamic resizable array in C. Array works with any struct
 * that have specific format.
 *
 * @date 2024-12-04
 * @copyright (C) 2024 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2024 WolodiaM
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
/**
 * @page DArrayHelp Dynamic array helper documentation
 * @section DynArray
 * All macroses operate on structs. To be qualified as DynArray struct must
 * match this pattern: format:
 * \code{.c}
 * struct {
 * 	T* data;
 * 	size_t size;
 * 	size_t capacity;
 * }
 * \endcode
 * T is @ref DynArrT
 * @section DynArrT
 * This datatype represents any datatype that user intends to store in the array
 */
// Code
#ifndef __CBUILD_DYN_ARRAY_H__
#define __CBUILD_DYN_ARRAY_H__
#include "common.h"
// Code
/**
 * @brief Append element to a dynamic array
 *
 * @param da => @ref DynArray * -> Dynamic array to operate on
 * @param val => @ref DynArrT -> Element that will be inserted into array
 */
#define cbuild_da_append(da, val)                                              \
	do {                                                                         \
		if ((da)->size >= (da)->capacity) {                                        \
			(da)->capacity = ((da)->capacity == 0) ? CBUILD_DA_INIT_CAPACITY         \
																						 : (da)->capacity * 2;             \
			(da)->data		 = realloc((da)->data, (da)->capacity);                    \
			assert((da)->data != NULL &&                                             \
						 "DynArray resize failed due to allocation error. Exiting...");    \
		}                                                                          \
		(da)->data[(da)->size] = (val);                                            \
		(da)->size++;                                                              \
	} while (0);
/**
 * @brief Append element to a dynamic array
 *
 * @param da => @ref DynArray * -> Dynamic array to operate on
 * @param vals => @ref DynArrT -> Array of elements that need to be inserted
 * @param vals_cnt => size_t -> Number of elements to insert
 */
#define cbuild_da_append_arr(da, vals, vals_cnt)                               \
	do {                                                                         \
		for (size_t i = 0; i < (vals_cnt); i++) {                                  \
			cbuild_da_append((da), (vals)[i]);                                       \
		}                                                                          \
	} while (0);
/**
 * @brief Append element to a dynamic array
 *
 * @param da => @ref DynArray * -> Dynamic array to operate on
 * @param elem_type => type -> Datatype that dynamic array stores,
 * needed because C preprocessor limitations
 * @param ... => @ref DynArrT ... -> Values that need to be inserted
 */
#define cbuild_da_append_many(da, elem_type, ...)                              \
	do {                                                                         \
		elem_type tmp[] = { __VA_ARGS__ };                                         \
		cbuild_da_append_arr((da), tmp, (size_t)(sizeof(tmp) / sizeof(tmp[0])));   \
	} while (0);
/**
 * @brief Clear dynamic array
 *
 * @param da => @ref DynArray * -> Dynamic array to operate on
 */
#define cbuild_da_clear(da)                                                    \
	(da)->size		 = 0;                                                          \
	(da)->capacity = 0;                                                          \
	free((da)->data);                                                            \
	(da)->data = NULL;
#endif // __CBUILD_DYN_ARRAY_H__
