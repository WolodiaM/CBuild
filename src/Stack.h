/**
 * @file Stack.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Simple stack implementation
 *
 * To create new stack datatype, you can use this code:
 * @code{.c}
 * struct my_stack_t {
 *	my_type_t* data;
 *	size_t ptr;
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
#ifndef __CBUILD_STACK_H__
#define __CBUILD_STACK_H__
// Project includes
#include "Log.h"
#include "common.h"
// Code
/**
 * @brief Push element to a stack
 *
 * @param stack => CBUILD_STACK* -> Stack
 * @param elem => VAL* -> Element
 */
#define cbuild_stack_push(stack, elem)                                         \
	do {                                                                         \
		if ((stack)->ptr >= (stack)->capacity) {                                   \
			cbuild_stack_resize((stack), 0);                                         \
		}                                                                          \
		(stack)->data[(stack)->ptr++] = elem;                                      \
	} while(0)
/**
 * @brief Pop element from a stack
 *
 * @param stack => CBUILD_STACK* -> Stack
 * @return VAL -> Element
*/
#define cbuild_stack_pop(stack)                                                \
	({                                                                           \
		cbuild_assert((stack)->ptr != 0, "(LIB_CBUILD_STACK) Stack underfloor.\n");\
		(stack)->data[--(stack)->ptr];                                             \
	})
/**
 * @brief Get top element of the stack (same as for pop, but read-only)
 *
 * @param stack => CBUILD_STACK* -> Stack
 * @return VAL -> Element
 */
#define cbuild_stack_top(stack)                                                \
	({                                                                           \
		cbuild_assert((stack)->ptr != 0, "(LIB_CBUILD_STACK) Stack underfloor.\n");\
		(stack)->data[(stack)->ptr - 1];                                           \
	})
/**
 * @brief Resize stack (done automatically most of the time ;) )
 *
 * @param stack => CBUILD_STACK* -> Stack
 * @param size => size_t -> New element count. Stack will be truncated if it
 * will be lover than stack->ptr. If it is zero then default behaviour is used
 */
#define cbuild_stack_resize(stack, size)                                       \
	do {                                                                         \
		if ((stack)->data == NULL) {                                               \
			if (size == 0) {                                                         \
				(stack)->capacity = CBUILD_INIT_CAPACITY;                              \
			} else {                                                                 \
				(stack)->capacity = size;                                              \
			}                                                                        \
			(stack)->data = cbuild_malloc(                                           \
			  (stack)->capacity * sizeof(typeof(*(stack)->data)));                   \
		} else {                                                                   \
			if (size == 0) {                                                         \
				(stack)->capacity *= 2;                                                \
			} else {                                                                 \
				(stack)->capacity = size;                                              \
			}                                                                        \
			(stack)->data = cbuild_realloc((stack)->data,                            \
			  (stack)->capacity * sizeof(typeof(*(stack)->data)));                   \
		}                                                                          \
		cbuild_assert((stack)->data != NULL,                                       \
		  "(LIB_CBUILD_STACK) Allocation failed.\n");                              \
	} while (0)
/**
 * @brief Free stack
 *
 * @param stack => CBUILD_STACK* -> Stack
 */
#define cbuild_stack_clear(stack)                                              \
	do {                                                                         \
		cbuild_free((stack)->data);                                                \
		(stack)->data     = NULL;                                                  \
		(stack)->ptr      = 0;                                                     \
		(stack)->capacity = 0;                                                     \
	} while(0)
#endif // __CBUILD_STACK_H__
