/**
 * @file DLload.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Wrapper for easy runtime loading of dynamic libraries
 * @date 2025-06-25
 *
 * @copyright (C) 2025 WolodiaM
 * @license GPL-3.0-or-later
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
#ifndef __CBUILD_DLLOAD_H__
#	if defined(CBUILD_API_POSIX)
#		define cbuild_dlib_handle                         void*
#		define cbuild_dlib_load(filename)                 dlopen(filename, RTLD_LAZY)
#		define cbuild_dlib_load_force(filename)           dlopen(filename, RTLD_NOW)
#		define cbuild_dlib_get_function(handle, funcName) dlsym(handle, funcName)
#		define cbuild_dlib_close(handle)                  dlclose(handle)
#	elif defined(CBUILD_API_WIN32)
#		define cbuild_dlib_handle                         HMODULE
#		define cbuild_dlib_load(filename)                 LoadLibrary(filename)
#		define cbuild_dlib_load_force(filename)           LoadLibrary(filename)
#		define cbuild_dlib_get_function(handle, funcName) GetProcAddress(handle, funcName)
#		define cbuild_dlib_close(handle)                  FreeLibrary(handle)
#	endif // CBUILD_API_*
#endif   // __CBUILD_DLLOAD_H__
