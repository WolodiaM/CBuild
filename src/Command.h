/**
 * @file Command.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Command runner
 * Allow to manage command buffers ad run them. Alows for sync/async run. With
 * shared io/piped io/no io.
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
// Code
#ifndef __CBUILD_COMMAND_H__
#define __CBUILD_COMMAND_H__
#include "DynArray.h"
#include "Map.h"
#include "StringBuilder.h"
#include "common.h"
#include "Proc.h"
// Command
typedef struct cbuild_cmd_t {
	const char** data;
	size_t size;
	size_t capacity;
} cbuild_cmd_t;
// IO overrides
typedef struct {
	cbuild_fd_t fdstdin;
	cbuild_fd_t fdstdout;
	cbuild_fd_t fdstderr;
} cbuild_cmd_fd_t;
/**
 * @brief Append new arg to cmd
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 * @param val => char* -> New token
 */
#define	cbuild_cmd_append(cmd, val) cbuild_da_append(cmd, val)
/**
 * @brief Append new args to cmd
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 * @param vals => char** -> New tokens
 * @param vals_cnt => size_t -> Count of new tokens
 */
#define	cbuild_cmd_append_arr(cmd, vals, vals_cnt)                             \
	cbuild_da_append_arr(cmd, vals, vals_cnt)
/**
 * @brief Append new args to cmd
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 * @param ... => char* -> New tokens
 */
#define	cbuild_cmd_append_many(cmd, ...)                                       \
	cbuild_da_append_many(cmd, __VA_ARGS__)
/**
 * @brief  Clear command buffer
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 */
#define	cbuild_cmd_clear(cmd) cbuild_da_clear(cmd)
/**
 * @brief Conver cbuild_cmd_t to cbuild_sb_t
 *
 * @param cmd => cbuild_cmd_t -> Command
 * @return CBuildStrBuff -> String buffer to work with
 */
CBUILDDEF cbuild_sb_t cbuild_cmd_to_sb(cbuild_cmd_t cmd);
/**
 * @brief Conver cbuild_cmd_t to cbuild_sb_t
 *
 * @param cmd => cbuild_cmd_t -> Command
 * @return CBuildStrBuff -> String buffer to work with
 */
#define	cbuild_sb_from_cmd(cmd) cbuild_cmd_to_sb(cmd)
/**
 * @brief Call async command without io redirecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @return cbuild_proc_t -> Process associated with called command
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
  CBUILDDEF cbuild_proc_t cbuild_cmd_async(cbuild_cmd_t cmd));
/**
 * @brief Call async command with io rediecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @param fd => cbuild_cmd_fd_t -> IO redicrecting table
 * @return cbuild_proc_t -> Process associated with called command
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
  CBUILDDEF cbuild_proc_t cbuild_cmd_async_redirect(cbuild_cmd_t cmd,
    cbuild_cmd_fd_t fd));
/**
 * @brief Call sync command without io redirecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @return true -> Command succeed
 * @return false -> Command failed
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
  CBUILDDEF bool cbuild_cmd_sync(cbuild_cmd_t cmd));
/**
 * @brief Call sync command with io rediecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @param df => cbuild_cmd_fd_t -> IO redicrecting table
 * @return true -> Command succeed
 * @return false -> Command failed
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
  CBUILDDEF bool cbuild_cmd_sync_redirect(cbuild_cmd_t cmd, cbuild_cmd_fd_t fd));
typedef struct cbuild_cmd_opt_t {
	// Redirect
	cbuild_fd_t* fdstdin;
	cbuild_fd_t* fdstdout;
	cbuild_fd_t* fdstderr;
	// Flags
	union {
		uint32_t flags;
		struct {
			uint32_t no_reset     : 1;
			uint32_t autokill     : 1;
			uint32_t pass_proc    : 1; // Implicit async
			uint32_t append_proc  : 1; // Implicit async
			uint32_t no_print_cmd : 1;
			uint32_t              : 27;
		};
	};
	// Misc options
	union {
		cbuild_proclist_t* procs;
		cbuild_proc_t* proc;
	};
} cbuild_cmd_opt_t;
/**
 * @brief Run commannd
 *
 * @brief cmd => cbuild_cmd_t* -> Command to execute
 * @brief opts => cbuild_cmd_opt_t -> Command options
 */
CBUILDDEF bool cbuild_cmd_run_opt(cbuild_cmd_t* cmd, cbuild_cmd_opt_t opts);
/**
 * @brief Run commannd
 *
 * @brief cmd => cbuild_cmd_t* -> Command to execute
 * @brief ... => cbuild_cmd_opt_t... -> Command options,
 * will be inserted in struct intiliazier.
 */
#define cbuild_cmd_run(cmd, ...)                                               \
	cbuild_cmd_run_opt(cmd, (cbuild_cmd_opt_t){ __VA_ARGS__ })
#endif // __CBUILD_COMMAND_H__
