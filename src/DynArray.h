/**
 * @file DynArray.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Dynamic array implementation
 *
 * To create new array datatype, you can use this code:
 * @code{.c}
 * struct my_array_t {
 *	my_type_t* data;
 *	size_t size;
 *	size_t capacity;
 * }
 * @endcode
 * Struct can contain other fields, if needed.
 * See the [CBuild Wiki](/3.examples/7.datatypes.html) for more datails.
 *
 * @date 2025-01-26
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
#ifndef __CBUILD_DYN_ARRAY_H__
#define __CBUILD_DYN_ARRAY_H__
// Project includes
#include "Log.h"
#include "common.h"
// Code
/**
 *  @brief Append element to a dynamic array
 *
 *  @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 *  @param elem => VAL -> Element
 */
#define cbuild_da_append(da, elem)                                             \
	do {                                                                         \
		if (((da)->size + 1) > (da)->capacity) {                                   \
			cbuild_da_resize((da), 0);                                               \
		}                                                                          \
		(da)->data[(da)->size++] = elem;                                           \
	} while(0)
/**
 * @brief Append an array to a da
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param arr => VAL* -> Array pointer
 * @param arr_size => size_t -> Number of new elements
 */
#define cbuild_da_append_arr(da, arr, arr_size)                                \
	do {                                                                         \
		if (((da)->size + (arr_size)) > (da)->capacity) {                          \
			cbuild_da_resize((da), (da)->capacity + (arr_size));                     \
		}                                                                          \
		memcpy((da)->data + (da)->size, (arr),                                     \
		  (arr_size) * sizeof(typeof(*(da)->data)));                               \
		(da)->size += (arr_size);                                                  \
	} while(0)
/**
 * @brief Append multiple elements to a da
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param ... => VAL -> New elements
 */
#define cbuild_da_append_many(da, ...)                                         \
	do {                                                                         \
		typeof(*((da)->data))   __cbuild_da_tmp_arr[] = { __VA_ARGS__ };           \
		size_t __cbuild_da_tmp_count =                                             \
		sizeof(__cbuild_da_tmp_arr) / sizeof(__cbuild_da_tmp_arr[0]);              \
		cbuild_da_append_arr((da), __cbuild_da_tmp_arr, __cbuild_da_tmp_count);    \
	} while (0)
/**
 * @brief Set da element using its index
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Index
 * @param elem => VAL -> Element
 * @return bool -> Success or failure on overflow
 */
#define cbuild_da_set(da, idx, elem)                                           \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_DA) Index overflow in set.");  \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			(da)->data[(idx)] = elem;                                                \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Get an element from a da using index
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Element index
 * @return VAL* -> Element or NULL on overflow
 */
#define cbuild_da_get(da, idx)                                                 \
	({                                                                           \
		typeof(*(da)->data)* __cbuild__ret = NULL;                                 \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_DA) Index overflow in get.");  \
			__cbuild__ret = NULL;                                                    \
		} else {                                                                   \
			__cbuild__ret = &((da)->data[(idx)]);                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Remove an element from a da using index
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Element index
 * @return bool -> Success or failure on overflow
 */
#define cbuild_da_remove(da, idx)                                              \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR,                                             \
			  "(LIB_CBUILD_DA) Index overflow in remove.");                          \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			memmove((da)->data + (idx), ((da)->data + ((idx) + 1)),                  \
			  ((da)->size - ((idx) + 1)) * sizeof(typeof(*(da)->data)));             \
			(da)->size--;                                                            \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Remove an element from a da using index
 * O(1) but order is changed
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Element index
 * @return bool -> Success or failure on overflow
 */
#define cbuild_da_remove_unordered(da, idx)                                    \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR,                                             \
			  "(LIB_CBUILD_DA) Index overflow in remove.");                          \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			(da)->data[(idx)] = (da)->data[--(da)->size];                            \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Resize da (done automatically most of the time ;) )
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param new_size => size_t -> New element count. Array will be truncated if it
 * will be lover than da->size. If zero then default growth behavior is used
 */
#define cbuild_da_resize(da, new_size)                                         \
	do {                                                                         \
		if ((da)->data == NULL) {                                                  \
			if ((new_size) == 0) {                                                   \
				(da)->capacity = CBUILD_INIT_CAPACITY;                                 \
			} else {                                                                 \
				(da)->capacity = (new_size);                                           \
			}                                                                        \
			(da)->data = cbuild_malloc(                                              \
			  (da)->capacity * sizeof(typeof(*(da)->data)));                         \
		} else {                                                                   \
			if ((new_size) == 0) {                                                   \
				(da)->capacity *= 2;                                                   \
			} else {                                                                 \
				(da)->capacity = (new_size);                                           \
			}                                                                        \
			(da)->data = cbuild_realloc((da)->data,                                  \
			  (da)->capacity * sizeof(typeof(*(da)->data)));                         \
		}                                                                          \
		cbuild_assert((da)->data != NULL, "(LIB_CBUILD_DA) Allocation failed.\n"); \
		((da)->size > (da)->capacity) ? ((da)->size = (da)->capacity) : 0;         \
	} while(0)
/**
 * @brief Free da
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 */
#define cbuild_da_clear(da)                                                    \
	do {                                                                         \
		cbuild_free((da)->data);                                                   \
		(da)->data     = NULL;                                                     \
		(da)->size     = 0;                                                        \
		(da)->capacity = 0;                                                        \
	} while(0)
/**
 * @brief Foreach loop
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param iter => NAME -> Iteration value name
 */
#define cbuild_da_foreach(da, iter)                                            \
	for (typeof(*((da)->data))* iter = (da)->data;                               \
	  iter < ((da)->data + (da)->size); iter++)
#endif // __CBUILD_DYN_ARRAY_H__
