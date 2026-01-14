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
 *  @file header-avl.h
 *  @brief add-on to header.h
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#ifndef HEADER_AVL_H
#define HEADER_AVL_H

#include "common.h"

void emit_aggregate_avl_typedefs(FILE *outfile, xmlNodePtr node, int indent);
void emit_aggregate_avl(FILE *outfile, xmlNodePtr node, int indent);
void emit_aggregate_avl_node(FILE *outfile, xmlNodePtr node, int indent);
void emit_aggregate_avl_function_prototypes(FILE *outfile,
                                            xmlNodePtr node,
                                            char *project_name);

#endif //HEADER_AVL_H
