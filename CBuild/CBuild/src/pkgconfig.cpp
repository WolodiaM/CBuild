/**
 * @file pkgconfig.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @date 2023-12-01
 * @brief Implementattion of pkg config API
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
// C++ libs
#include "string"
// Project headers
#include "../../headers/pkgconfig.hpp"
#include "../../headers/print.hpp"
#include "../../headers/system.hpp"
/* pkgconfig */
bool CBuild::get_pkg_info(CBuild::package_info* package) {
    bool disabled = false;
    if (!CBuild::is_system_enabled()) {
        CBuild::enable_system();
        disabled = true;
    }
    CBuild::print("Searching pkg-config for package \"" + package->name + "\"", CBuild::MAGENTA);
    std::string largs = CBuild::system_piped("pkg-config --libs " + package->name, 64 * 1024);
    if (largs[0] != '-') {
        CBuild::print("Package \"" + package->name + "\" not found!", CBuild::RED);
        package->largs = "";
        package->cargs = "";
        if (disabled) {
            CBuild::disable_system();
        }
        return false;
    }
    package->largs = largs;
    package->cargs = CBuild::system_piped("pkg-config --cflags " + package->name, 64 * 1024);
    if (disabled) {
        CBuild::disable_system();
    }
    if (package->cargs.size() > 3) {
        package->cargs = package->cargs.substr(0, package->cargs.size() - 1);
    } else {
        package->cargs = "";
    }
    package->largs = package->largs.substr(0, package->largs.size() - 1);
    return true;
}
