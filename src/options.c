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
 *
 *  @brief tracks code generation options
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "options.h"

  /**
   *  @fn annotation_type option_annotation(options *opts)
   *
   *  @brief  returns annotation type
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current @a annotation_type
   */

annotation_type option_annotation(options *opts)
{
  return opts ? opts->annotation_type : annotation_type_none;
}

  /**
   *  @fn void option_set_annotation(options *opts, const char *type)
   *
   *  @brief  sets annotation type
   *
   *  @param  opts - pointer to @a options struct
   *  @param  type - string representation of @a annotation_type
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_annotation(options *opts, const char *type)
{
  if (opts) opts->annotation_type = annotation_string_to_type(type);
}

  /**
   *  @fn license_type option_license(options *opts)
   *
   *  @brief  returns license type
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current @a license_type
   */

license_type option_license(options *opts)
{
  return opts ? opts->license_type : license_type_none;
}

  /**
   *  @fn void option_set_license(options *opts, const char *type)
   *
   *  @brief  sets license type
   *
   *  @param  opts - pointer to @a options struct
   *  @param  type - string representation of @a license_type
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_license(options *opts, const char *type)
{
  if (opts) opts->license_type = license_string_to_type(type);
}

  /**
   *  @fn void option_set_generator_options(options *opts,
   *                                        const char *generators)
   *
   *  @brief  turns on optional code generators
   *
   *  @param  opts - pointer to @a options struct
   *  @param  generators - comma separated list of:
   *                       array
   *                       list
   *                       avl
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_generator_options(options *opts, const char *generators)
{
  char *opt = NULL;

  option_gen_array_off(opts);
  option_gen_list_off(opts);
  option_gen_avl_off(opts);

  if (!generators) return;

  for (opt = strtok((char *)generators, ","); opt; opt = strtok(NULL, ","))
  {
    if (!strcasecmp(opt, "array")) option_gen_array_on(opts);
    else if (!strcasecmp(opt, "list")) option_gen_list_on(opts);
    else if (!strcasecmp(opt, "avl")) option_gen_avl_on(opts);
  }
}

  /**
   *  @fn char *option_makefile_cc(options *opts)
   *
   *  @brief  returns makefile CC setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return string with current makefile CC setting
   */

char *option_makefile_cc(options *opts)
{
  return opts ? opts->makefile_cc : NULL;
}

  /**
   *  @fn char *option_makefile_copts(options *opts)
   *
   *  @brief  returns makefile COPTS setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return string with current makefile COPTS setting
   */

char *option_makefile_copts(options *opts)
{
  return opts ? opts->makefile_copts : NULL;
}

  /**
   *  @fn char *option_makefile_install_dir(options *opts)
   *
   *  @brief  returns makefile INSTALL_DIR setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return string with current makefile CC setting
   */

char *option_makefile_install_dir(options *opts)
{
  return opts ? opts->makefile_install_dir : NULL;
}

  /**
   *  @fn bool option_gen_makefile(options *opts)
   *
   *  @brief  returns gen makefile setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current makefile generation setting
   */

bool option_gen_makefile(options *opts)
{
  return opts ? opts->gen_makefile : false;
}

  /**
   *  @fn void option_gen_makefile_on(options *opts)
   *
   *  @brief  turns makefile generation on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_makefile_on(options *opts)
{
  if (opts) opts->gen_makefile = true;
}

  /**
   *  @fn void option_gen_makefile_off(options *opts)
   *
   *  @brief  turns makefile generation off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_makefile_off(options *opts)
{
  if (opts) opts->gen_makefile = true;
}

  /**
   *  @fn void option_set_makefile_options(options *opts, const char *optlist);
   *
   *  @brief  tracks makefile generator options
   *
   *  @param  opts - pointer to @a options struct
   *  @param  optlist - comma separated list of:
   *            CC=&lt;compiler&gt;
   *            COPTS=&lt;options&gt;
   *            INSTALL_DIR=&lt;base directory of installation&gt;
   *                            ie.  '/usr/local'
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_makefile_options(options *opts, const char *optlist)
{
  char *opt = NULL;
  char *val = NULL;

  if (!opts || !optlist) return;

  for (opt = strtok((char *)optlist, ","); opt; opt = strtok(NULL, ","))
  {
    val = strchr(opt, '=');
    if (val)
    {
      ++val;
      if (!strncasecmp(opt, "CC", 2))
          strncpy(opts->makefile_cc, val, 255);
      else if (!strncasecmp(opt, "COPTS", 5))
          strncpy(opts->makefile_copts, val, 255);
      else if (!strncasecmp(opt, "INSTALL_DIR", 11))
          strncpy(opts->makefile_install_dir, val, 255);
    }
  }
}

  /**
   *  @fn bool option_gen_array(options *opts)
   *
   *  @brief  returns gen array setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current array generation setting
   */

bool option_gen_array(options *opts)
{
  return opts ? opts->gen_array : false;
}

  /**
   *  @fn void option_gen_array_on(options *opts)
   *
   *  @brief  turns array generation on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_array_on(options *opts)
{
  if (opts) opts->gen_array = true;
}

  /**
   *  @fn void option_gen_array_off(options *opts)
   *
   *  @brief  turns array generation off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_array_off(options *opts)
{
  if (opts) opts->gen_array = false;
}

  /**
   *  @fn bool option_gen_list(options *opts)
   *
   *  @brief  returns gen list setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current list generation setting
   */

bool option_gen_list(options *opts)
{
  return opts ? opts->gen_list : false;
}

  /**
   *  @fn void option_gen_list_on(options *opts)
   *
   *  @brief  turns list generation on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_list_on(options *opts)
{
  if (opts) opts->gen_list = true;
}

  /**
   *  @fn void option_gen_list_off(options *opts)
   *
   *  @brief  turns list generation off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_list_off(options *opts)
{
  if (opts) opts->gen_list = false;
}

  /**
   *  @fn bool option_gen_avl(options *opts)
   *
   *  @brief  returns gen avl setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current avl generation setting
   */

bool option_gen_avl(options *opts)
{
  return opts ? opts->gen_avl : false;
}

  /**
   *  @fn void option_gen_avl_on(options *opts)
   *
   *  @brief  turns avl generation on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_avl_on(options *opts)
{
  if (opts) opts->gen_avl = true;
}

  /**
   *  @fn void option_gen_avl_off(options *opts)
   *
   *  @brief  turns avl generation off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_avl_off(options *opts)
{
  if (opts) opts->gen_avl = false;
}

  /**
   *  @fn bool option_gen_readme(options *opts)
   *
   *  @brief  returns gen readme setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current readme generation setting
   */

bool option_gen_readme(options *opts)
{
  return opts ? opts->gen_readme : false;
}

  /**
   *  @fn void option_gen_readme_on(options *opts)
   *
   *  @brief  turns readme generation on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_readme_on(options *opts)
{
  if (opts) opts->gen_readme = true;
}

  /**
   *  @fn void option_gen_readme_off(options *opts)
   *
   *  @brief  turns readme generation off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_gen_readme_off(options *opts)
{
  if (opts) opts->gen_readme = false;
}

  /**
   *  @fn bool option_assume_typedefs(options *opts)
   *
   *  @brief  returns gen readme setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current readme generation setting
   */

bool option_assume_typedefs(options *opts)
{
  return opts ? opts->assume_typedefs : false;
}

  /**
   *  @fn void option_assume_typedefs_on(options *opts)
   *
   *  @brief  turns readme generation on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_assume_typedefs_on(options *opts)
{
  if (opts) opts->assume_typedefs = true;
}

  /**
   *  @fn void option_assume_typedefs_off(options *opts)
   *
   *  @brief  turns readme generation off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_assume_typedefs_off(options *opts)
{
  if (opts) opts->assume_typedefs = false;
}

  /**
   *  @fn void option_set_includes(options *opts, const char *inc_list);
   *
   *  @brief  sets list of include files to be emitted in header
   *
   *  @param opts - pointer to @a options struct
   *  @param inc_list - ':' separated list of include file names
   *
   *  @par Returns
   *       Nothing.
   */

void option_set_includes(options *opts, const char *inc_list)
{
  char *fn;
  char *fn_end;
  int n_fns = 0;
  int i;

  if (!opts) goto exit;

  if (opts->n_include_files)
  {
    for (i = 0; i < opts->n_include_files; i++)
      free(opts->include_files[i]);
    free(opts->include_files);
    opts->include_files = NULL;
    opts->n_include_files = 0;
  }

  if (!inc_list) goto exit;

  n_fns = 1;

  fn = (char *)inc_list;
  while (*fn)
  {
    if (*fn == ':') ++n_fns;
    ++fn;
  }

  opts->include_files = malloc(sizeof(char *) * (n_fns + 1));
  if (!opts->include_files) goto exit;
  opts->n_include_files = n_fns;
  memset(opts->include_files, 0, sizeof(char *) * (n_fns + 1));

  for (fn = fn_end = (char *)inc_list, i = 0; i < n_fns; i++)
  {
    while (*fn_end && (*fn_end != ':')) ++fn_end;
    opts->include_files[i] = strndup(fn, fn_end - fn);
    if (*fn_end == ':') ++fn_end;
    fn = fn_end;
  }

exit:
  return;
}

  /**
   *  @fn char *option_get_first_include(options *opts);
   *
   *  @brief  returns first include file in list
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return string containing include file name
   */

char *option_get_first_include(options *opts)
{
  if (!opts) return NULL;
  if (!opts->n_include_files) return NULL;

  opts->curr_include_file = 0;

  return opts->include_files[opts->curr_include_file];
}

  /**
   *  @fn char *option_get_next_include(options *opts);
   *
   *  @brief  returns next include file in list
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return string containing include file name
   */

char *option_get_next_include(options *opts)
{
  if (!opts) return NULL;
  if (opts->curr_include_file >= opts->n_include_files) return NULL;
  if (!opts->include_files[opts->curr_include_file]) return NULL;

  return opts->include_files[opts->curr_include_file++];
}

  /**
   *  @fn bool option_cpp_compatible(options *opts)
   *
   *  @brief  returns cpp compatible setting
   *
   *  @param opts - pointer to @a options struct
   *
   *  @return current cpp compatible setting
   */

bool option_cpp_compatible(options *opts)
{
  return opts ? opts->cpp_compatible : false;
}

  /**
   *  @fn void option_cpp_compatible_on(options *opts)
   *
   *  @brief  turns cpp compatibility on
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_cpp_compatible_on(options *opts)
{
  if (opts) opts->cpp_compatible = true;
}

  /**
   *  @fn void option_cpp_compatible_off(options *opts)
   *
   *  @brief  turns cpp compatibility off
   *
   *  @param opts - pointer to @a options struct
   *
   *  @par Returns
   *       Nothing.
   */

void option_cpp_compatible_off(options *opts)
{
  if (opts) opts->cpp_compatible = false;
}

