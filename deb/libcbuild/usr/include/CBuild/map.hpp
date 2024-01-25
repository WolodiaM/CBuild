/**
 * @file map.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Custom implementation of map datatype
 * @version 1.0
 * @date 2023-01-04
 *
 *
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2023  WolodiaM
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
#ifndef __MYMAP_HPP__
#define __MYMAP_HPP__
// C++ libraries
#include "stdexcept"
#include "vector"
// Code
namespace lib {
/**
 * @brief Simple pair of values, have "==" operator implemented
 * @tparam _K -> Key, if this will be added to map need to have "==" operator
 * implemented
 * @tparam _D -> Value
 * @n All types must implement "=" operator
 * @n All types must implement "==" operator
 */
template <class _K, class _D> class mapData {
  public:
    _K key;
    _D data;

  public:
    /**
     * @brief Construct a new map data object, @n
     * ! Key and data variables is not initialize and contains garbage
     */
    mapData() {}
    /**
     * @brief Construct a new map data object
     *
     * @param key => _K -> Key
     * @param data => _D -> Value
     */
    mapData(_K key, _D data) {
        this->key = key;
        this->data = data;
    }
    /**
     * @brief If ((this.key == val.key) && (this.data == val.data))
     * @param val => lib::mapData<_K, _D> -> Another mapData
     * @return true -> Two object is equals
     * @return false -> Two object is different
     */
    bool operator==(lib::mapData<_K, _D> val) {
        if ((this->key == val.key) && (this->data == val.data)) {
            return true;
        } else {
            return false;
        }
    }
};
/**
 * @brief Simple map implementation with some stack operation added
 * @tparam _K -> Key, must have "==" operator implemented
 * @tparam _D -> Value
 * @n All types must implement "=" operator
 */
template <class _K, class _D> class map {
  private:
    std::vector<mapData<_K, _D>> content;

  public:
    /**
     * @brief Construct a new map object
     */
    map() {
        this->content.clear();
    }
    /**
     * @brief Copy constructor
     */
    map(map const&) = default;
    /**
     * @brief Consruct a new map object and initialize it
     *
     * @param init_list => std::initializer_list -> New map data
     */
    map(std::initializer_list<lib::mapData<_K, _D>> init_list) {
        this->content.reserve(init_list.size()); // Reserve space for efficiency
        for (const auto& element : init_list) {
            this->content.push_back(element);
        }
    }
    /**
     * @brief Push new element to map, stack operation
     * ! Dangerous not perform chak if this elemnt exists in map
     * @param element => lib::mapData<_K, _D> -> mapData object, that pack key
     * and value to one variable
     */
    void push_back(lib::mapData<_K, _D> element) {
        this->content.push_back(element);
    }
    /**
     * @brief Push new element to map, stack operation
     * ! Dangerous not perform chak if this elemnt exists in map
     * @param key => _K -> Key
     * @param data => _D -> Value
     */
    void push_back(_K key, _D data) {
        lib::mapData<_K, _D> element(key, data);
        this->content.push_back(element);
    }
    /**
     * @brief Add data, but check if key exists, if exist - trow exception
     * @exception runtime_error -> string with error descitption
     * @param element => lib::mapData<_K, _D> -> New element to push
     */
    void push_back_check(lib::mapData<_K, _D> element) {
        if (this->get(element.key) != NULL)
            throw std::runtime_error("lib::map - Key already exist!");
        this->push_back(element);
    }
    /**
     * @brief Add data, but check if key exists, if exist - trow exception
     * @exception runtime_error -> string with error descitption
     * @param key => _K -> key
     * @param data => _D -> Data
     */
    void push_back_check(_K key, _D data) {
        if (this->get(key) != NULL)
            throw std::runtime_error("lib::map - Key already exist!");
        this->push_back(key, data);
    }
    /**
     * @brief Pop last element, remove it and return back it's copy
     * @return lib::mapData<_K, _D> -> Returned element
     */
    lib::mapData<_K, _D> pop_back() {
        lib::mapData<_K, _D> ret = this->content.at(this->content.size() - 1);
        this->content.pop_back();
        return ret;
    }
    /**
     * @brief Get element by it's key
     * @param key => _D -> Key
     * @return const _D* -> Returned element const reference
     */
    const _D* get(_K key) {
        if (this->content.empty())
            return NULL;
        for (unsigned int i = 0; i < this->content.size(); i++) {
            lib::mapData<_K, _D>* elem = &(this->content.at(i));
            if (elem->key == key) {
                return &(elem->data);
            }
        }
        return NULL;
    }
    /**
     * @brief Get reference to key and value of map
     * @param key => _K -> key that is needed
     * @return lib::mapData<_K, _D>* -> returned reference
     */
    lib::mapData<_K, _D>* get_ptr(_K key) {
        if (this->content.empty())
            return NULL;
        for (unsigned int i = 0; i < this->content.size(); i++) {
            lib::mapData<_K, _D>* elem = &(this->content.at(i));
            if (elem->key == key) {
                return elem;
            }
        }
        return NULL;
    }
    /**
     * @brief Does map contains some key
     * @param key => _K -> Key
     * @return true -> Contains
     * @return false -> Not contains
     */
    bool contains_key(_K key) {
        for (lib::mapData<_K, _D> data : this->content) {
            if (data.key == key)
                return true;
        }
        return false;
    }
    /**
     * @brief Does map contains some value
     * @param value => _D -> Value
     * @return true -> Contains
     * @return false -> Not contains
     */
    bool contains(_D value) {
        for (lib::mapData<_K, _D> data : this->content) {
            if (data.data == value)
                return true;
        }
        return false;
    }
    /**
     * @brief Erase all elements (simmilar to std::vector::clear())
     */
    void clear() {
        this->content.clear();
    }
    /**
     * @brief Array operation, get element at index
     * @param i => size_t -> index
     * @return lib::mapData<_K, _D> -> returned element
     */
    lib::mapData<_K, _D> at(__SIZE_TYPE__ i) {
        return this->content.at(i);
    }
    /**
     * @brief Array operation, get pointer to an element at specified index
     *
     * @param i => size_t -> Index
     * @return lib::mapData<_K, _D>* -> Returned element pointer
     */
    lib::mapData<_K, _D>* ptr_at(__SIZE_TYPE__ i) {
        return &(this->content.at(i));
    }
    /**
     * @brief Remove element from map and return copy of this element
     * @param idx Index of element to remove
     */
    void remove(__SIZE_TYPE__ idx) {
        if (idx >= this->content.size())
            return;
        this->content.erase(std::next(this->content.begin(), idx));
    }
    /**
     * @brief Remove element with given key
     * @param key => _K -> Key of elent that is needed to be removed
     */
    void remove(_K key) {
        __SIZE_TYPE__ idx = this->content.size();
        for (unsigned int i = 0; i < this->content.size(); i++) {
            lib::mapData<_K, _D> elem = this->content.at(i);
            if (elem.key == key) {
                idx = i;
            }
        }
        if (idx >= this->content.size())
            return;
        this->content.erase(std::next(this->content.begin(), idx));
    }
    /**
     * @brief Get keys list
     * @return std::vector<_K> -> list of keys
     */
    std::vector<_K> keys() {
        std::vector<_K> ret;
        for (auto elem : this->content) {
            ret.push_back(elem.key);
        }
        return ret;
    }
    /**
     * @brief Size of map
     * @return size_t -> Number of elements
     */
    __SIZE_TYPE__ size() {
        return this->content.size();
    }
    /**
     * @brief Typical array operation, simmilar to at but using operator []
     * @param i => size_t -> Index
     * @return lib::mapData<_K, _D> -> Returned element
     */
    lib::mapData<_K, _D> operator[](__SIZE_TYPE__ i) {
        return this->content.at(i);
    }
    /**
     * @brief Add one elment to map
     * @param val => lib::mapData<_K, _D> -> element
     * @return lib::map<_K, _D> -> map
     */
    lib::map<_K, _D> operator+(lib::mapData<_K, _D> val) {
        lib::map<_K, _D> map;
        map.content = this->content;
        map.content.push_back(val);
        return map;
    }
    /**
     * @brief Add one element to map, modify it
     * @param val => lib::mapData<_K, _D> -> element
     * @return lib::map<_K, _D> -> map
     */
    lib::map<_K, _D> operator+=(lib::mapData<_K, _D> val) {
        lib::map<_K, _D> map;
        map.content = this->content;
        map.content.push_back(val);
        this->content = map.content;
        return map;
    }
    /**
     * @brief Add another map to this map
     * @param val => lib::map<_K, _D> -> another map
     * @return lib::map<_K, _D> -> map
     */
    lib::map<_K, _D> operator+(lib::map<_K, _D> val) {
        lib::map<_K, _D> map;
        map.content = this->content;
        for (lib::mapData<_K, _D> elem : val.content) {
            map.push(elem);
        }
        return map;
    }
    /**
     * @brief Add another map to this, modify it
     * @param val => lib::map<_K, _D> -> another map
     * @return lib::map<_K, _D> -> map
     */
    lib::map<_K, _D> operator+=(lib::map<_K, _D> val) {
        lib::map<_K, _D> map;
        map.content = this->content;
        for (lib::mapData<_K, _D> elem : val.content) {
            map.push(elem);
        }
        this->content = map.content;
        return map;
    }
    /**
     * @brief Copy content of second map to first
     * @param val => lib::map<_K, _D> -> another map
     * @return lib::map<_K, _D> -> map
     */
    lib::map<_K, _D> operator=(lib::map<_K, _D> val) {
        this->content = val.content;
        return *this;
    }
    /**
     * @brief Reset map, set first it's element to val
     * @param val => lib::mapData<_K, _D> -> mapData variable
     * @return lib::map<_K, _D> -> map
     */
    lib::map<_K, _D> operator=(lib::mapData<_K, _D> val) {
        this->content.clear();
        this->content.push_back(val);
        return *this;
    }
    /**
     * @brief Check if content of two maps is equal
     * @param val => lib::map<_K, _D> -> map variable
     * @return bool -> standart comparion rules
     */
    bool operator==(lib::map<_K, _D> val) {
        if (this->content.size() != val.content.size())
            return false;
        for (__SIZE_TYPE__ i = 0; i < this->content.size(); i++) {
            if (this->content.at(i) != val.content.at(i))
                return false;
        }
        return true;
    }
    /**
     * @brief Begin iterator
     * @return std::vector<mapData<_K, _D>>::iterator -> begin iterator
     */
    typename std::vector<mapData<_K, _D>>::iterator begin() {
        return this->content.begin();
    }
    /**
     * @brief End iterator
     * @return std::vector<mapData<_K, _D>>::iterator -> end iterator
     */
    typename std::vector<mapData<_K, _D>>::iterator end() {
        return this->content.end();
    }
    /**
     * @brief Begin const iterator
     * @return std::vector<mapData<_K, _D>>::const_iterator -> begin const
     * iterator
     */
    typename std::vector<mapData<_K, _D>>::const_iterator begin() const {
        return this->content.begin();
    }
    /**
     * @brief End const iterator
     * @return std::vector<mapData<_K, _D>>::const_iterator -> end const
     * iterator
     */
    typename std::vector<mapData<_K, _D>>::const_iterator end() const {
        return this->content.end();
    }
    /**
     * @brief Simmilar to std::vector::empty
     */
    bool empty() {
        return this->content.empty();
    }
    void erase(_K key) {
        for (unsigned int i = 0; i < this->content.size(); i++) {
            if (this->content.at(i).key == key) {
                this->content.erase(this->content.begin() + i);
            }
        }
    }
};
} // namespace lib
#endif // __MYMAP_HPP__
