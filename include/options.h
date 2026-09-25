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
 *  @file annotation.h
 *  @brief tracks code generation annotation setting
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

#include "license.h"
#include "annotation.h"

  /**
   *  @typedef options
   *  @brief creates a type for an @a options struct
   */

typedef struct options options;

  /**
   *  @struct options
   *  @brief defines a list of aggregate names
   *
   *  NOTE:  aggregate is one of a struct or union
   */

struct options
{
  bool gen_makefile;
  char makefile_cc[256];
  char makefile_copts[256];
  char makefile_install_dir[256];
  bool gen_array;
  bool gen_list;
  bool gen_avl;
  bool gen_readme;
  bool assume_typedefs;
  annotation_type annotation_type;
  license_type license_type;
  char **include_files;
  unsigned n_include_files;
  unsigned curr_include_file;
  bool cpp_compatible;
};

annotation_type option_annotation(options *opts);
void option_set_annotation(options *opts, const char *type);

license_type option_license(options *opts);
void option_set_license(options *opts, const char *type);

void option_set_generator_options(options *opts, const char *generators);

char *option_makefile_cc(options *opts);
char *option_makefile_copts(options *opts);
char *option_makefile_install_dir(options *opts);
bool option_gen_makefile(options *opts);
void option_gen_makefile_on(options *opts);
void option_gen_makefile_off(options *opts);
void option_set_makefile_options(options *opts, const char *optlist);

bool option_gen_array(options *opts);
void option_gen_array_on(options *opts);
void option_gen_array_off(options *opts);

bool option_gen_list(options *opts);
void option_gen_list_on(options *opts);
void option_gen_list_off(options *opts);

bool option_gen_avl(options *opts);
void option_gen_avl_on(options *opts);
void option_gen_avl_off(options *opts);

bool option_gen_readme(options *opts);
void option_gen_readme_on(options *opts);
void option_gen_readme_off(options *opts);

bool option_assume_typedefs(options *opts);
void option_assume_typedefs_on(options *opts);
void option_assume_typedefs_off(options *opts);

void option_set_includes(options *opts, const char *inc_list);
char *option_get_first_include(options *opts);
char *option_get_next_include(options *opts);

bool option_cpp_compatible(options *opts);
void option_cpp_compatible_on(options *opts);
void option_cpp_compatible_off(options *opts);

#endif //OPTIONS_H

