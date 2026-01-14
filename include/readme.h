/*
 *  Copyright 2025,2026 Patrick T. Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 *  @file readme.h
 *  @brief builds Doxygen configuration file
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is Doxygen configuration file
 */

#ifndef README_H
#define README_H

#include "common.h"

void gen_readme(xmlDocPtr doc, char *base_name);

#endif //README_H
