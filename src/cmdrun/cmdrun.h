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
#include "../array/array.h"
#	include "../stringbuilder/sb.h"
#include "sys/types.h"
typedef int LibCMDRunFD;
#define LIB_CMDRUN_INVALID_FD -1
typedef pid_t LibCMDRunProc;
#define LIB_CMDRUN_INVALID_PROC -1
da_t(char**, cmdrun_char_ptr_ptr);
da_t_impl(char**, cmdrun_char_ptr_ptr);
typedef da_cmdrun_char_ptr_ptr_t cmd_t;
typedef struct {
	LibCMDRunFD fdstdin;
	LibCMDRunFD fdstdout;
	LibCMDRunFD fdstderr;
} cmd_fd_redirect_t;
/**
 * @brief Append new arg to cmd
 *
 * @param cmd => cmd_t* -> Command bufer to work with
 * @param val => char* -> New token
 */
#define cmd_append(cmd, val) da_append(cmd, val)
/**
 * @brief Append new args to cmd
 *
 * @param cmd => cmd_t* -> Command bufer to work with
 * @param vals => char** -> New tokens
 * @param vals_cnt => size_t -> Count of new tokens
 */
#define cmd_append_arr(cmd, vals, vals_cnt)                             \
	cbuild_da_append_arr(cmd, vals, vals_cnt)
/**
 * @brief Append new args to cmd
 *
 * @param cmd => cmd_t* -> Command bufer to work with
 * @param ... => char* -> New tokens
 */
#define cbuild_cmd_append_many(cmd, ...)                                       \
	cbuild_da_append_many(cmd, char*, __VA_ARGS__)
/**
 * @brief  Clear command buffer
 *
 * @param cmd => cmd_t* -> Command bufer to work with
 */
#define cbuild_cmd_clear(cmd) cbuild_da_clear(cmd)
/**
 * @brief Conver cmd_t to CBuildStrBuff
 *
 * @param cmd => cmd_t -> Command
 * @param sb => CBuildStrBuff -> String buffer to work with
 */
void cbuild_cmd_to_sb(cmd_t cmd, CBuildStrBuff* sb);
/**
 * @brief Call async command without io rediecting
 *
 * @param cmd => cmd_t -> Command buffer
 * @return LibCMDRunProc -> Process associated with called command
 */
#define cbuild_cmd_async(cmd)                                                  \
	cbuild_cmd_async_redirect(cmd, (cmd_fd_redirect_t){ CBUILD_INVALID_FD,     \
																												CBUILD_INVALID_FD,     \
																												CBUILD_INVALID_FD })
/**
 * @brief Call async command with io rediecting
 *
 * @param cmd => cmd_t -> Command buffer
 * @param df => cmd_fd_redirect_t -> IO redicrecting table
 * @return LibCMDRunProc -> Process associated with called command
 */
LibCMDRunProc cbuild_cmd_async_redirect(cmd_t cmd, cmd_fd_redirect_t fd);
/**
 * @brief Call sync command without io rediecting
 *
 * @param cmd => cmd_t -> Command buffer
 * @return true -> Command succeed
 * @return false -> Command failed
 */
#define cbuild_cmd_sync(cmd)                                                   \
	cbuild_cmd_sync_redirect(cmd, (cmd_fd_redirect_t){ CBUILD_INVALID_FD,      \
																											 CBUILD_INVALID_FD,      \
																											 CBUILD_INVALID_FD })
/**
 * @brief Call sync command with io rediecting
 *
 * @param cmd => cmd_t -> Command buffer
 * @param df => cmd_fd_redirect_t -> IO redicrecting table
 * @return true -> Command succeed
 * @return false -> Command failed
 */
bool cbuild_cmd_sync_redirect(cmd_t cmd, cmd_fd_redirect_t fd);
#endif // __CBUILD_COMMAND_H__
