/*
 *  Copyright 2025,2026 Patrick T. Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 *  @file common.h
 *  @brief Routines common to all kahdifire modules
 */

#ifndef COMMON_H
#define COMMON_H

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "strapp.h"

    /* declarations for arrays type */

  /**
   *  @typedef arrays
   *  @brief creates a type for an @a arrays struct
   */

typedef struct arrays arrays;

  /**
   *  @struct arrays
   *  @brief defines a list of array subscript sizes
   */

struct arrays
{
  int n;       /**<  number of items in @a array     */
  int *array;  /**<  array of array subscript sizes  */
};

arrays *arrays_new(void);
void arrays_free(arrays *as);
void arrays_add(arrays *as, int n_elements);

  /* declarations for aggregates type */

  /**
   *  @typedef aggregates
   *  @brief creates a type for an @a aggregates struct
   */

typedef struct aggregates aggregates;

  /**
   *  @struct aggregates
   *  @brief defines a list of aggregate names
   *
   *  NOTE:  aggregate is one of a struct or union
   */

struct aggregates
{
  int n;         /**<  number of items in @a array                         */
  char **array;  /**<  array of strings each containing an aggregate name  */
};

extern aggregates *type_cache;

aggregates *aggregates_new(void);
void aggregates_free(aggregates *ags);
void aggregates_add(aggregates *ags, char *name);
int aggregates_find(aggregates *ags, char *name);


  /* Function prototypes for general purpose use  */

    /* Functions for high level use */

int gen_code(char *file_name, char *base_name);

    /* Functions to enhance XML parsing  */

xmlDocPtr parse_xml(char *buf);
char *get_attribute(xmlNodePtr node, char *attr_name);

    /* Functions to manipulate strings */

char *get_project_name(char *base_name);
void str_upper(char *str);
void str_lower(char *str);
char *create_base_name(char *file_name);
char *function_prefix(char *project, char *declaration);

    /* Miscellaneous functions */

size_t get_file_size(char *file_name);
void emit_indent(FILE *outfile, int indent);

    /* Functions to assist in XML parsing */

int pointer_count(xmlNodePtr node);
xmlNodePtr pointer_find_scalar(xmlNodePtr node);
xmlNodePtr pointer_find_reference(xmlNodePtr node);
arrays *pointer_find_array(xmlNodePtr node);
arrays *array_levels(xmlNodePtr node);
void array_level(arrays *arrs, xmlNodePtr node);
int array_level_count(xmlNodePtr node);
int array_pointer_count(xmlNodePtr node);
xmlNodePtr array_find_scalar(xmlNodePtr node);
xmlNodePtr array_find_reference(xmlNodePtr node);

#endif //COMMON_H
