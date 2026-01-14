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
 *  @file options.h
 *  @brief tracks code generation options
 */

#include <stdio.h>
#include <string.h>

#include "config.h"

#include "options.h"

  /**
   *  @fn annotation_type option_annotation(void)
   *  @brief  returns annotation type
   *
   *  @par Parameters
   *       None.
   *
   *  @return current @a annotation_type
   */

annotation_type option_annotation(void) { return annotation_get_type(); }

  /**
   *  @fn void option_set_annotation(char *type)
   *  @brief  sets annotation type
   *
   *  @param  type - string representation of @a annotation_type
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_annotation(char *type)
{
  annotation_set_type(annotation_string_to_type(type));
}

  /**
   *  @fn license_type option_license(void)
   *  @brief  returns license type
   *
   *  @par Parameters
   *       None.
   *
   *  @return current @a license_type
   */

license_type option_license(void) { return license_get_type(); }

  /**
   *  @fn void option_set_license(char *type)
   *  @brief  sets license type
   *
   *  @param  type - string representation of @a license_type
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_license(char *type)
{
  license_set_type(license_string_to_type(type));
}

  /**
   *  @fn void option_set_generator_options(char *generators)
   *  @brief  turns on optional code generators
   *
   *  @param  generators - comma separated list of:
   *                       array
   *                       list
   *                       avl
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_generator_options(char *generators)
{
  char *opt = NULL;

  option_gen_array_off();
  option_gen_list_off();
  option_gen_avl_off();

  if (!generators) return;

  for (opt = strtok(generators, ","); opt; opt = strtok(NULL, ","))
  {
    if (!strcasecmp(opt, "array")) option_gen_array_on();
    else if (!strcasecmp(opt, "list")) option_gen_list_on();
    else if (!strcasecmp(opt, "avl")) option_gen_avl_on();
  }
}

static bool _gen_makefile = false;
static char *_makefile_cc = "gcc";
static char *_makefile_copts = "-Wall -O3 -g0";
static char *_makefile_install_dir = "/usr/local";

  /**
   *  @fn char *option_makefile_cc(void)
   *  @brief  returns makefile CC setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return string with current makefile CC setting
   */

char *option_makefile_cc(void) { return _makefile_cc; }

  /**
   *  @fn char *option_makefile_copts(void)
   *  @brief  returns makefile COPTS setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return string with current makefile COPTS setting
   */

char *option_makefile_copts(void) { return _makefile_copts; }

  /**
   *  @fn char *option_makefile_install_dir(void)
   *  @brief  returns makefile INSTALL_DIR setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return string with current makefile CC setting
   */

char *option_makefile_install_dir(void) { return _makefile_install_dir; }

  /**
   *  @fn bool option_gen_makefile(void)
   *  @brief  returns gen makefile setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return current makefile generation setting
   */

bool option_gen_makefile(void) { return _gen_makefile; }

  /**
   *  @fn void option_gen_makefile_on(void)
   *  @brief  turns makefile generation on
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_makefile_on(void) { _gen_makefile = true; }

  /**
   *  @fn void option_gen_makefile_off(void)
   *  @brief  turns makefile generation off
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_makefile_off(void) { _gen_makefile = false; }

  /**
   *  @fn void option_set_makefile_options(char *options);
   *  @brief  tracks makefile generator options
   *
   *  @param  options - comma separated list of:
   *            CC=&lt;compiler&gt;
   *            COPTS=&lt;options&gt;
   *            INSTALL_DIR=&lt;base directory of installation, ie. '/usr/local'&gt;
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_makefile_options(char *options)
{
  char *opt = NULL;

  option_gen_array_off();
  option_gen_list_off();
  option_gen_avl_off();

  if (!options) return;

  for (opt = strtok(options, ","); opt; opt = strtok(NULL, ","))
  {
    if (!strncasecmp(opt, "CC", 2))
    {
      opt = strtok(NULL, "=");
      _makefile_cc = strdup(opt);
    }
    else if (!strncasecmp(opt, "COPTS", 5))
    {
      opt = strtok(NULL, "=");
      _makefile_copts = strdup(opt);
    }
    else if (!strncasecmp(opt, "INSTALL_DIR", 11))
    {
      opt = strtok(NULL, "=");
      _makefile_install_dir = strdup(opt);
    }
  }
}

static bool _gen_array = false;

  /**
   *  @fn bool option_gen_array(void)
   *  @brief  returns gen array setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return current array generation setting
   */

bool option_gen_array(void) { return _gen_array; }

  /**
   *  @fn void option_gen_array_on(void)
   *  @brief  turns array generation on
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_array_on(void) { _gen_array = true; }

  /**
   *  @fn void option_gen_array_off(void)
   *  @brief  turns array generation off
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_array_off(void) { _gen_array = false; }

static bool _gen_list = false;

  /**
   *  @fn bool option_gen_list(void)
   *  @brief  returns gen list setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return current list generation setting
   */

bool option_gen_list(void) { return _gen_list; }

  /**
   *  @fn void option_gen_list_on(void)
   *  @brief  turns list generation on
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_list_on(void) { _gen_list = true; }

  /**
   *  @fn void option_gen_list_off(void)
   *  @brief  turns list generation off
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_list_off(void) { _gen_list = false; }

static bool _gen_avl = false;

  /**
   *  @fn bool option_gen_avl(void)
   *  @brief  returns gen avl setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return current avl generation setting
   */

bool option_gen_avl(void) { return _gen_avl; }

  /**
   *  @fn void option_gen_avl_on(void)
   *  @brief  turns avl generation on
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_avl_on(void) { _gen_avl = true; }

  /**
   *  @fn void option_gen_avl_off(void)
   *  @brief  turns avl generation off
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_avl_off(void) { _gen_avl = false; }

static bool _gen_readme = false;

  /**
   *  @fn bool option_gen_readme(void)
   *  @brief  returns gen readme setting
   *
   *  @par Parameters
   *       None.
   *
   *  @return current readme generation setting
   */

bool option_gen_readme(void) { return _gen_readme; }

  /**
   *  @fn void option_gen_readme_on(void)
   *  @brief  turns readme generation on
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_readme_on(void) { _gen_readme = true; }

  /**
   *  @fn void option_gen_readme_off(void)
   *  @brief  turns readme generation off
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_readme_off(void) { _gen_readme = false; }

