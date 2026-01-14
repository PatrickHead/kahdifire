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

annotation_type option_annotation(void);
void option_set_annotation(char *type);

license_type option_license(void);
void option_set_license(char *type);

void option_set_generator_options(char *generators);

char *option_makefile_cc(void);
char *option_makefile_copts(void);
char *option_makefile_install_dir(void);
bool option_gen_makefile(void);
void option_gen_makefile_on(void);
void option_gen_makefile_off(void);
void option_set_makefile_options(char *generators);

bool option_gen_array(void);
void option_gen_array_on(void);
void option_gen_array_off(void);

bool option_gen_list(void);
void option_gen_list_on(void);
void option_gen_list_off(void);

bool option_gen_avl(void);
void option_gen_avl_on(void);
void option_gen_avl_off(void);

bool option_gen_readme(void);
void option_gen_readme_on(void);
void option_gen_readme_off(void);

#endif //OPTIONS_H

