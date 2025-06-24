/**
 * @file Stack.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Simple stack implementation
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
#ifndef __CBUILD_STACK_H__
#define __CBUILD_STACK_H__
// Project includes
#include "Log.h"
#include "common.h"
// Code
#define cbuild_stack_t(V, Vname)                                               \
	typedef void (*cbuild_stack_##Vname##_push_t)(void* self, V elem);           \
	typedef V (*cbuild_stack_##Vname##_pop_t)(void* self);                       \
	typedef void (*cbuild_stack_##Vname##_resize_t)(void* self, size_t size);    \
	typedef void (*cbuild_stack_##Vname##_clear_t)(void* self);                  \
	typedef void* (*cbuild_stack_##Vname##_malloc_t)(size_t bytes);              \
	typedef void* (*cbuild_stack_##Vname##_realloc_t)(void* ptr, size_t bytes);  \
	typedef void (*cbuild_stack_##Vname##_free_t)(void* ptr);                    \
	typedef struct cbuild_stack_##Vname##_t {                                    \
		V*                               data;                                     \
		size_t                           ptr;                                      \
		size_t                           capacity;                                 \
		cbuild_stack_##Vname##_push_t    push;                                     \
		cbuild_stack_##Vname##_pop_t     pop;                                      \
		cbuild_stack_##Vname##_resize_t  resize;                                   \
		cbuild_stack_##Vname##_clear_t   clear;                                    \
		cbuild_stack_##Vname##_malloc_t  malloc;                                   \
		cbuild_stack_##Vname##_realloc_t realloc;                                  \
		cbuild_stack_##Vname##_free_t    free;                                     \
	} cbuild_stack_##Vname##_t
#define cbuild_stack_t_ext_impl(Vname)                                         \
	extern const cbuild_stack_##Vname##_t cbuild_stack_##Vname
#define cbuild_stack_t_impl(V, Vname)                                          \
	void cbuild_stack_##Vname##_push(void* s, V elem) {                          \
		cbuild_stack_##Vname##_t* self = s;                                        \
		if ((self->ptr + 1) > self->capacity) {                                    \
			self->resize(self, 0);                                                   \
		}                                                                          \
		self->data[self->ptr++] = elem;                                            \
	}                                                                            \
	V cbuild_stack_##Vname##_pop(void* s) {                                      \
		cbuild_stack_##Vname##_t* self = s;                                        \
		cbuild_assert(self->ptr != 0, "(LIB_CBUILD_STACK) Stack underfloor.\n");   \
		return self->data[--self->ptr];                                            \
	}                                                                            \
	void cbuild_stack_##Vname##_resize(void* s, size_t size) {                   \
		cbuild_stack_##Vname##_t* self = s;                                        \
		if (self->data == NULL) {                                                  \
			if (size == 0) {                                                         \
				self->capacity = CBUILD_INIT_CAPACITY;                                 \
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
		cbuild_assert(self->data != NULL,                                          \
		              "(LIB_CBUILD_STACK) Allocation failed.\n");                  \
	}                                                                            \
	void cbuild_stack_##Vname##_clear(void* s) {                                 \
		cbuild_stack_##Vname##_t* self = s;                                        \
		self->free(self->data);                                                    \
		self->data     = NULL;                                                     \
		self->ptr      = 0;                                                        \
		self->capacity = 0;                                                        \
	}                                                                            \
	const cbuild_stack_##Vname##_t cbuild_stack_##Vname = {                      \
		.data     = NULL,                                                          \
		.ptr      = 0,                                                             \
		.capacity = 0,                                                             \
		.push     = cbuild_stack_##Vname##_push,                                   \
		.pop      = cbuild_stack_##Vname##_pop,                                    \
		.resize   = cbuild_stack_##Vname##_resize,                                 \
		.clear    = cbuild_stack_##Vname##_clear,                                  \
		.malloc   = __CBUILD_MALLOC,                                               \
		.realloc  = __CBUILD_REALLOC,                                              \
		.free     = __CBUILD_FREE,                                                 \
	}
/**
 * @brief Push element to a stack
 *
 * @param stack => STACK* -> Stack
 * @param elem => VAL* -> Element
 */
#define cbuild_stack_push(stack, elem)   (stack)->push((stack), (elem))
/**
 * @brief Pop element from a stack
 *
 * @param stack => CBUILD_STACK* -> Stack
 * @return VAL -> Element
 */
#define cbuild_stack_pop(stack)          (stack)->pop((stack))
/**
 * @brief Resize stack (done automatically most of the time ;) )
 *
 * @param stack => CBUILD_STACK* -> Stack
 * @param size => size_t -> New element count. Stack will be truncated if it
 * will be lover than stack->ptr. If it is zero then default behaviour is used
 */
#define cbuild_stack_resize(stack, size) (stack)->resize((stack), (size))
/**
 * @brief Free stack
 *
 * @param stack => CBUILD_STACK* -> Stack
 */
#define cbuild_stack_clear(stack)        (stack)->clear((stack))
#endif // __CBUILD_STACK_H__
