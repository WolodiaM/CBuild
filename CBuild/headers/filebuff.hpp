/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Filebuffer for CBuild ecosystem
 * @date Wed May  3 01:44:19 PM EEST 2023
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
#include "fstream"
#include "iostream"
#include "list"
#include "sstream"
#include "string"
#include "string_view"
#include "vector"
// Code
#ifndef __FILEBUFF_HPP__
#define __FILEBUFF_HPP__
namespace CBuild {
/**
 * @brief Parse string to vector of strings
 *
 * @param in => std::string -> Input string
 * @return std::vector<std::string> -> Returned list of lines
 */
std::vector<std::string> parse_lines(std::string in) {
  std::vector<std::string> ret;
  std::string buff;
  for (char c : in) {
    if (c == '\n') {
      ret.push_back(buff);
      buff.clear();
    } else {
      buff.push_back(c);
    }
  }
  if (buff.length() > 0) {
    ret.push_back(buff);
  }
  return ret;
}
/**
 * @brief Represent atate of buffer
 */
enum buffer_state {
  /**
   * @brief File match buffer
   */
  MATCH,
  /**
   * @brief File is newer, buffer is not updated
   */
  FILE_NEWER,
  /**
   * @brief Buffer is newer and file need update
   */
  BUFFER_NEWER
};
/**
 * @class filebuff
 * @brief Base class of filebuffer
 */
class filebuff {
protected:
  /**
   * @brief Path to file
   */
  std::string path;
  /**
   * @brief Does file need to be automaticly
   */
  bool autorefresh;
  /**
   * @brief Currennt state of buffer. All operation that change buffer
   * need to set state to CBuild::BUFFER_NEWER or update file based on
   * autorefresh variable. All function that update buffer/file and
   * synchronise it need to set state to CBuild::MATCH.
   */
  CBuild::buffer_state state;
  /**
   * @brief Load buffer from file
   */
  virtual void load_buffer() = 0;
  /**
   * @brief Save buffer to file
   */
  virtual void save_buffer() = 0;

public:
  /**
   * @brief Create filebuffer, simply store some vars
   *
   * @param file => std::string -> Path to file
   * @param autorefresh => bool -> set autorefresh var
   */
  filebuff(std::string file, bool autorefresh = true) {
    this->path = file;
    this->autorefresh = autorefresh;
    this->state = CBuild::buffer_state::FILE_NEWER;
  }
  /**
   * @brief Get character from file, return -1 on error
   *
   * @param pos => unsigned int -> Position of character in file
   * @return char -> Symbol
   */
  virtual char get_char(unsigned int pos) = 0;
  /**
   * @brief Get sequence of characters from file
   *
   * @param pos => unsigned int -> Postion of first character of line
   * @param size => unsigned int -> Number of characters in line
   * @return std::string -> String
   */
  virtual std::string get_str(unsigned int pos, unsigned int size) = 0;
  /**
   * @brief Set character in file
   *
   * @param ch => char -> Character
   * @param pos => unsigned int -> Character will be appended to file on
   * this pos
   */
  virtual void set_char(char ch, unsigned int pos) = 0;
  /**
   * @brief Set string in file
   *
   * @param str => std::string -> String
   * @param pos => unsigned int -> String will be appended to file on this
   * pos
   */
  virtual void set_str(std::string str, unsigned int pos) = 0;
  /**
   * @brief Remove character from file
   *
   * @param pos => unsigned int -> Position of character thet need to be
   * removed
   */
  virtual void del_char(unsigned int pos) = 0;
  /**
   * @brief Remove sequence of characters from file
   *
   * @param pos => unsigned int -> Postion of first character of sequence
   * @param size => unsigned int -> Number of character to remove
   */
  virtual void del_str(unsigned int pos, unsigned int size) = 0;
  /**
   * @brief Update file/buffer based on internal state variable
   */
  virtual void update() = 0;
  /**
   * @brief Get state of file
   *
   * @return CBuild::buffer_state -> Current state of filebuffer
   */
  CBuild::buffer_state get_state() { return this->state; }
};
class line_filebuff : public CBuild::filebuff {
protected:
  std::list<std::string> buff;
  void load_buffer() {
    std::ifstream file(this->path);
    std::string line;
    while (std::getline(file, line)) {
      this->buff.push_back(line);
    }
    file.close();
    this->state = CBuild::MATCH;
  }
  void save_buffer() {
    std::ofstream file(this->path);
    for (auto line : this->buff) {
      file << line << "\n";
    }
  }

public:
  line_filebuff(std::string file, bool refresh = false, bool new_file = false)
      : filebuff(file, refresh) {
    if (!new_file) {
      this->load_buffer();
    }
  }
  char get_char(unsigned int pos) {
    signed long tmp_pos = pos;
    for (auto line : this->buff) {
      if ((tmp_pos - (signed long)line.length()) < 0) {
        return line.at(tmp_pos);
      } else {
        tmp_pos = tmp_pos - (signed long)line.length();
      }
    }
    return -1;
  }
  std::string get_str(unsigned int pos, unsigned int size) {
    signed long tmp_pos = pos;
    std::string ret = "";
    for (unsigned int i = 0; i < this->buff.size(); i++) {
      auto it = this->buff.begin();
      std::advance(it, i);
      std::string line = *it;
      // std::string line = this->buff.at(i);
      if ((tmp_pos - (signed long)line.length()) < 0) {
        signed long size_tmp = size;
        bool first = true;
        while (true) {
          if ((size_tmp - (signed long)line.length()) < 0) {
            if (first) {
              ret += line.substr(tmp_pos, size_tmp);
              return ret;
            } else {
              ret += line.substr(0, size_tmp);
              return ret;
            }
          } else {
            if (first) {
              ret += line.substr(tmp_pos);
            } else {
              ret += line;
            }
            i++;
            auto it = this->buff.begin();
            std::advance(it, i);
            line = *it;
          }
          first = false;
          size_tmp = size_tmp - line.length();
        }
      } else {
        tmp_pos = tmp_pos - (signed long)line.length();
      }
    }
    return std::string("");
  }
  void set_char(char ch, unsigned int pos) {
    signed long tmp_pos = pos;
    for (unsigned int i = 0; i < this->buff.size(); i++) {
      auto it = this->buff.begin();
      std::advance(it, i);
      std::string line = *it;
      if ((tmp_pos - (signed long)line.length()) < 0) {
        std::string start, end;
        start = line.substr(0, tmp_pos);
        end = line.substr(pos);
        it->clear();
        it->append(start);
        it->push_back(ch);
        it->append(end);
        goto end;
      } else {
        tmp_pos = tmp_pos - (signed long)line.length();
      }
    }
  end:
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void set_str(std::string str, unsigned int pos) {
    signed long tmp_pos = pos;
    for (unsigned int i = 0; i < this->buff.size(); i++) {
      auto it = this->buff.begin();
      std::advance(it, i);
      std::string line = *it;
      if ((tmp_pos - (signed long)line.length()) < 0) {
        std::string start, end;
        start = line.substr(0, tmp_pos);
        end = line.substr(pos);
        it->clear();
        it->append(start);
        std::vector<std::string> processed_str = parse_lines(str);
        it->append(processed_str.at(0));
        for (unsigned int j = 1; j < processed_str.size(); j++) {
          std::advance(it, 1);
          this->buff.insert(it, processed_str.at(j));
        }
        it->append(end);
        goto end;
      } else {
        tmp_pos = tmp_pos - (signed long)line.length();
      }
    }
  end:
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void del_char(unsigned int pos) {
    signed long tmp_pos = pos;
    for (unsigned int i = 0; i < this->buff.size(); i++) {
      auto it = this->buff.begin();
      std::advance(it, i);
      std::string line = *it;
      if ((tmp_pos - (signed long)line.length()) < 0) {
        std::string start, end;
        start = line.substr(0, tmp_pos);
        end = line.substr(tmp_pos + 1);
        it->clear();
        it->append(start);
        it->append(end);
        goto end;
      } else {
        tmp_pos = tmp_pos - (signed long)line.length();
      }
    }
  end:
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void del_str(unsigned int pos, unsigned int size) {
    signed long tmp_pos = pos;
    for (unsigned int i = 0; i < this->buff.size(); i++) {
      auto it = this->buff.begin();
      std::advance(it, i);
      std::string line = *it;
      if ((tmp_pos - (signed long)line.length()) < 0) {
        bool multiline = false;
        std::string start, end;
        start = line.substr(0, tmp_pos);
        unsigned int del_size = 0;
        if (line.length() - pos > size) {
          end = line.substr(tmp_pos + size + 1);
        } else {
          end = "";
          multiline = true;
          del_size = line.substr(tmp_pos).length();
        }
        it->clear();
        it->append(start);
        it->append(end);
        if (multiline) {
          signed long tmp_size = size - del_size;
          while (true) {
            i++;
            auto it = this->buff.begin();
            std::advance(it, i);
            std::string line = *it;
            if ((tmp_size - (signed long)line.length()) < 0) {
              std::string end = line.substr(tmp_size);
              it->clear();
              it->append(end);
              goto end;
            } else {
              this->buff.erase(it);
            }
            tmp_size = tmp_size - line.length();
          }
        }
        goto end;
      } else {
        tmp_pos = tmp_pos - (signed long)line.length();
      }
    }
  end:
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  /**
   * @brief Get line from buffer
   *
   * @param pos => unsigned int -> Linenumber
   * @return std::string -> Line
   */
  std::string get_line(unsigned int pos) {
    auto it = this->buff.begin();
    std::advance(it, pos);
    return *it;
  }
  /**
   * @brief Set line in file
   *
   * @param str => std::string -> Line
   * @param pos => unsigned int -> Linenumber of new line
   */
  void set_line(std::string str, unsigned int pos) {
    auto it = this->buff.begin();
    std::advance(it, pos);
    this->buff.insert(it, str);
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  /**
   * @brief Delete line from file
   *
   * @param pos => unsigned int -> Linenumber
   */
  void del_line(unsigned int pos) {
    auto it = this->buff.begin();
    std::advance(it, pos);
    this->buff.erase(it);
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void update() {
    if (this->state == CBuild::MATCH) {
      return;
    } else if (this->state == CBuild::BUFFER_NEWER) {
      this->save_buffer();
      return;
    } else if (this->state == CBuild::FILE_NEWER) {
      this->load_buffer();
      return;
    } else {
      return;
    }
  }
};
class str_filebuff : public CBuild::filebuff {
protected:
  std::string buff;
  void load_buffer() {
    std::ifstream file(this->path);
    std::stringstream str;
    str << file.rdbuf();
    this->buff = str.str();
    this->state = CBuild::buffer_state::MATCH;
  }
  void save_buffer() {
    std::ofstream file(this->path);
    file.write(this->buff.c_str(), this->buff.length());
    this->state = CBuild::buffer_state::MATCH;
  }

public:
  str_filebuff(std::string file, bool refresh = false, bool new_file = false)
      : filebuff(file, refresh) {
    if (!new_file) {
      this->load_buffer();
    }
  }
  char get_char(unsigned int pos) {
    if (pos < this->buff.length()) {
      return this->buff.at(pos);
    } else {
      return -1;
    }
  }
  std::string get_str(unsigned int pos, unsigned int size) {
    if ((pos + size) < this->buff.length()) {
      return this->buff.substr(pos, pos + size);
    } else if (pos < this->buff.length()) {
      return this->buff.substr(pos);
    } else {
      return std::string("");
    }
  }
  void set_char(char ch, unsigned int pos) {
    if (pos < this->buff.length()) {
      std::string start, end;
      start = this->buff.substr(0, pos);
      end = this->buff.substr(pos);
      this->buff = start + ch + end;
    }
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void set_str(std::string str, unsigned int pos) {
    std::string start, end;
    start = this->buff.substr(0, pos);
    end = this->buff.substr(pos);
    this->buff = start + str + end;
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void del_char(unsigned int pos) {
    std::string start, end;
    start = this->buff.substr(0, pos);
    end = this->buff.substr(pos + 1);
    this->buff = start + end;
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void del_str(unsigned int pos, unsigned int size) {
    std::string start, end;
    start = this->buff.substr(0, pos);
    end = this->buff.substr(pos + size + 1);
    this->buff = start + end;
    if (this->autorefresh) {
      this->save_buffer();
    } else {
      this->state = CBuild::BUFFER_NEWER;
    }
  }
  void update() {
    if (this->state == CBuild::MATCH) {
      return;
    } else if (this->state == CBuild::BUFFER_NEWER) {
      this->save_buffer();
      return;
    } else if (this->state == CBuild::FILE_NEWER) {
      this->load_buffer();
      return;
    } else {
      return;
    }
  }
};
} // namespace CBuild
#endif // __FILEBUFF_HPP__
