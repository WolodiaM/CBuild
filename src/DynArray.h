/**
 * @file DynArray.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Dynamic array implementation
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
#include "common.h"
// Code
#define cbuild_da_t(V, Vname)                                                  \
	typedef void (*cbuild_da_##Vname##_append_t)(void* self, V elem);            \
	typedef void (*cbuild_da_##Vname##_append_arr_t)(void* self, const V* arr,   \
	                                                 size_t size);               \
	typedef void (*cbuild_da_##Vname##_set_t)(void* self, size_t idx, V elem);   \
	typedef V* (*cbuild_da_##Vname##_get_t)(void* self, size_t idx);             \
	typedef void (*cbuild_da_##Vname##_remove_t)(void* self, size_t idx);        \
	typedef void (*cbuild_da_##Vname##_resize_t)(void*  self,                    \
	                                             size_t new_capacity);           \
	typedef void (*cbuild_da_##Vname##_clear_t)(void* self);                     \
	typedef void* (*cbuild_da_##Vname##_malloc_t)(size_t bytes);                 \
	typedef void* (*cbuild_da_##Vname##_realloc_t)(void* ptr, size_t bytes);     \
	typedef void (*cbuild_da_##Vname##_free_t)(void* ptr);                       \
	typedef void* (*cbuild_da_##Vname##_memcpy_t)(                               \
			void* restrict dest, const void* restrict src, size_t bytes);            \
	typedef struct cbuild_da_##Vname##_t {                                       \
		V*                               data;                                     \
		size_t                           size;                                     \
		size_t                           capacity;                                 \
		cbuild_da_##Vname##_append_t     append;                                   \
		cbuild_da_##Vname##_append_arr_t append_arr;                               \
		cbuild_da_##Vname##_set_t        set;                                      \
		cbuild_da_##Vname##_get_t        get;                                      \
		cbuild_da_##Vname##_remove_t     remove;                                   \
		cbuild_da_##Vname##_resize_t     resize;                                   \
		cbuild_da_##Vname##_clear_t      clear;                                    \
		cbuild_da_##Vname##_malloc_t     malloc;                                   \
		cbuild_da_##Vname##_realloc_t    realloc;                                  \
		cbuild_da_##Vname##_free_t       free;                                     \
		cbuild_da_##Vname##_memcpy_t     memcpy;                                   \
	} cbuild_da_##Vname##_t
#define cbuild_da_t_ext_impl(Vname)                                            \
	extern const cbuild_da_##Vname##_t cbuild_da_##Vname
#define cbuild_da_t_impl(V, Vname)                                             \
	void cbuild_da_##Vname##_append(void* s, V elem) {                           \
		cbuild_da_##Vname##_t* self = s;                                           \
		if ((self->size + 1) > self->capacity) {                                   \
			self->resize(self, 0);                                                   \
		}                                                                          \
		self->data[self->size++] = elem;                                           \
	}                                                                            \
	void cbuild_da_##Vname##_resize(void* s, size_t size) {                      \
		cbuild_da_##Vname##_t* self = s;                                           \
		if (self->data == NULL) {                                                  \
			if (size == 0) {                                                         \
				self->capacity = CBUILD_DA_INIT_CAPACITY;                              \
			} else {                                                                 \
				self->capacity = size;                                                 \
			}                                                                        \
			self->data = self->malloc(self->capacity * sizeof(V));                   \
		} else {                                                                   \
			if (size == 0) {                                                         \
				self->capacity *= 2;                                                   \
			} else {                                                                 \
				self->capacity = size;                                                 \
			}                                                                        \
			self->data = self->realloc(self->data, self->capacity * sizeof(V));      \
		}                                                                          \
		if (self->data == NULL) {                                                  \
			__CBUILD_ERR_PRINT("error: (LIB_CBUILD_DA) Allocation failed\n");        \
			exit(1);                                                                 \
		}                                                                          \
	}                                                                            \
	void cbuild_da_##Vname##_append_arr(void* s, const V* arr, size_t size) {    \
		cbuild_da_##Vname##_t* self = s;                                           \
		if ((self->size + size) > self->capacity) {                                \
			self->resize(self, self->capacity + size);                               \
		}                                                                          \
		self->memcpy(self->data + self->size, arr, size * sizeof(V));              \
		self->size += size;                                                        \
	}                                                                            \
	void cbuild_da_##Vname##_set(void* s, size_t idx, V elem) {                  \
		cbuild_da_##Vname##_t* self = s;                                           \
		if (idx >= self->size) {                                                   \
			__CBUILD_ERR_PRINT("error: (LIB_CBUILD_DA) Index overflow in set\n");    \
			exit(1);                                                                 \
		}                                                                          \
		self->data[idx] = elem;                                                    \
	}                                                                            \
	V* cbuild_da_##Vname##_get(void* s, size_t idx) {                            \
		cbuild_da_##Vname##_t* self = s;                                           \
		if (idx >= self->size) {                                                   \
			__CBUILD_ERR_PRINT("error: (LIB_CBUILD_DA) Index oveflow in set\n");     \
			exit(1);                                                                 \
		}                                                                          \
		return &self->data[idx];                                                   \
	}                                                                            \
	void cbuild_da_##Vname##_remove(void* s, size_t idx) {                       \
		cbuild_da_##Vname##_t* self = s;                                           \
		if (idx >= self->size) {                                                   \
			__CBUILD_ERR_PRINT("error: (LIB_CBUILD_DA) Index overflow in set\n");    \
			exit(1);                                                                 \
		}                                                                          \
		self->memcpy(self->data + idx, (self->data + (idx + 1)),                   \
		             (self->size - (idx + 1)) * sizeof(V));                        \
		self->size--;                                                              \
	}                                                                            \
	void cbuild_da_##Vname##_clear(void* s) {                                    \
		cbuild_da_##Vname##_t* self = s;                                           \
		self->free(self->data);                                                    \
		self->data     = NULL;                                                     \
		self->size     = 0;                                                        \
		self->capacity = 0;                                                        \
	}                                                                            \
	const cbuild_da_##Vname##_t cbuild_da_##Vname = {                            \
		.data       = NULL,                                                        \
		.size       = 0,                                                           \
		.capacity   = 0,                                                           \
		.append     = cbuild_da_##Vname##_append,                                  \
		.append_arr = cbuild_da_##Vname##_append_arr,                              \
		.set        = cbuild_da_##Vname##_set,                                     \
		.get        = cbuild_da_##Vname##_get,                                     \
		.remove     = cbuild_da_##Vname##_remove,                                  \
		.resize     = cbuild_da_##Vname##_resize,                                  \
		.clear      = cbuild_da_##Vname##_clear,                                   \
		.malloc     = __CBUILD_MALLOC,                                             \
		.realloc    = __CBUILD_REALLOC,                                            \
		.free       = __CBUILD_FREE,                                               \
		.memcpy     = __CBUILD_MEMCPY,                                             \
	}
/**
 *  @brief Append element to a dynamic array
 *
 *  @param da => CBUILD_DA* -> Dynamic array
 *  @param elem => VAL -> Element
 */
#define cbuild_da_append(da, elem) (da)->append((da), (elem))
/**
 * @brief Append an array to a da
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param arr => VAL* -> Array pointer
 * @param size => size_t -> Number of new elements
 */
#define cbuild_da_append_arr(da, arr, size)                                    \
	(da)->append_arr((da), (arr), (size))
/**
 * @brief Append multiple elements to a da
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param type => TYPE -> Type of array, limitations of C
 * @param ... => VAL -> New elements
 */
#define cbuild_da_append_many(da, type, ...)                                   \
	do {                                                                         \
		type   _tmp_arr[] = { __VA_ARGS__ };                                       \
		size_t _count     = sizeof(_tmp_arr) / sizeof(_tmp_arr[0]);                \
		cbuild_da_append_arr((da), _tmp_arr, _count);                              \
	} while (0)
/**
 * @brief Set da element using its index
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param idx => size_t -> Index
 * @param elem => VAL -> Element
 */
#define cbuild_da_set(da, idx, elem) (da)->set((da), (idx), (elem))
/**
 * @brief Get an element from a da using index
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param idx => size_t -> Element index
 * @return VAL* -> Element
 */
#define cbuild_da_get(da, idx)       (da)->get((da), (idx))
/**
 * @brief Remove an element from a da using index
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param idx => size_t -> Element index
 */
#define cbuild_da_remove(da, idx)    (da)->remove((da), (idx))
/**
 * @brief Resize da (done automatically most of the time ;) )
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param size => size_t -> New element count. Array will be truncated if it
 * will be lover than da->size. If it is zero then default behavior is used
 */
#define cbuild_da_resize(da, size)   (da)->resize((da), (size))
/**
 * @brief Free da
 *
 * @param da => CBUILD_DA* -> Dynamic array
 */
#define cbuild_da_clear(da)          (da)->clear((da))
/**
 * @brief Foreach loop
 *
 * @param da => CBUILD_DA* -> Dynamic array
 * @param type => TYPE -> Type of arrat. Limitations of C
 * @param iter => NAME -> Iteration value name
 */
#define cbuild_da_foreach(da, type, iter)                                      \
	for (type* iter = (da)->data; iter < ((da)->data + (da)->size); iter++)
#endif // __CBUILD_DYN_ARRAY_H__
