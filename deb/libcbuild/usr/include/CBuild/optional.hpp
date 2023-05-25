/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Optional datatype
 * @date Tue May 23 10:44:52 AM EEST 2023
 *
 *
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2022  WolodiaM
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
// C++ libraries
#include "stdexcept"
// Code
#ifndef __OPTIONAL_HPP__
#define __OPTIONAL_HPP__
namespace lib {
/**
 * @brief Optional data type
 *
 * @tparam T -> Type of internal data
 */
template <class T>
class optional {
       private:
	/**
	 * @brief Intrnal variable
	 */
	T var;
	/**
	 * @brief Does values is saved
	 */
	bool null;

       public:
	/**
	 * @brief Create new optional datatype
	 */
	optional() { this->null = true; }
	/**
	 * @brief Create new optional datatype
	 *
	 * @param val => T -> Value
	 */
	optional(T val) {
		this->var = val;
		this->null = false;
	}
	/**
	 * @brief Set value
	 *
	 * @param val => T -> Value
	 */
	void set(T val) {
		this->var = val;
		this->null = false;
	}
	/**
	 * @brief Clear value
	 */
	void clear() { this->null = true; }
	/**
	 * @brief Get value
	 * @throw std::runtime_error -> If type is null
	 * @return T -> Value, if any
	 */
	T get() {
		if (this->null == false) {
			return this->var;
		} else {
			throw new std::runtime_error("Datatype is null");
		}
	}
	/**
	 * @brief If value is not null
	 *
	 * @return bool -> true / false
	 */
	bool is() { return !this->null; }
};
}  // namespace lib
#endif	// __OPTIONAL_HPP__
