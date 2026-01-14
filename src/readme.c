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
 *  @file readme.c
 *  @brief Converts C declarations to readme
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is readme
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>

#include "config.h"

#include "readme.h"
#include "options.h"

  /*  Module specific function prototypes  */

void emit_readme(FILE *outfile, char *project_name);

/**
 *  @fn void gen_readme(xmlDocPtr doc, char *base_name)
 *
 *  @brief generates readme file
 *
 *  @param doc - xmlDocPtr containing declaration metadata
 *  @param base_name - basic name of project for output files
 *
 *  @par Returns
 *  Nothing.
 */

void gen_readme(xmlDocPtr doc, char *base_name)
{
  xmlNodePtr root;
  FILE *outfile = NULL;
  char *tmp;
  char *base_dir = NULL;
  char *outfile_name = NULL;
  char *project_name = NULL;

  if (!doc || !base_name) goto exit;

  if (!option_gen_readme()) goto exit;

  root = xmlDocGetRootElement(doc);
  if (!root) goto exit;

  if (!root->children) goto exit;

  if (strcmp((char *)root->name, "c-decls")) goto exit;

  tmp = strdup(base_name);
  if (!tmp) goto exit;

  base_dir = dirname(tmp);

  project_name = get_project_name(base_name);
  if (!project_name) goto exit;

  outfile_name = malloc(strlen(base_dir) +
                        strlen("Doxygen") +
                        strlen(project_name) +
                        3);
  if (!outfile_name) goto exit;

  sprintf(outfile_name, "%s/README.md", base_dir);

  outfile = fopen(outfile_name, "w");
  if (!outfile) goto exit;

  emit_readme(outfile, project_name);

exit:
  if (outfile) fclose(outfile);
  if (outfile_name) free(outfile_name);
  if (project_name) free(project_name);
  if (base_dir) free(base_dir);
}

static char *_readme =  /**<  format string for readme */
  "# %s - library to handle enums, structs and unions\n"
  "\n"
  "USER SUPPLIED DESCRIPTION GOES HERE\n"
  "\n"
  "<a id=\"TOC\"></a>\n"
  "## Table of contents\n"
  "\n"
  "* [Introduction](#introduction)\n"
  "* [Installation](#installation)\n"
  "* [Issues and Limitations](#known-issues-and-limitations)\n"
  "* [Getting Help](#getting-help)\n"
  "* [License Terms](#license)\n"
  "\n"
  "<a id=\"introduction\"></a>\n"
  "## Introduction\n"
  "\n"
  "USER SUPPLIED INTRODUCTION GOES HERE\n"
  "\n"
  "[Back to Table of Contents](#TOC)\n"
  "\n"
  "<a id=\"installation\"></a>\n"
  "## Installation\n"
  "\n"
  "#### Steps:\n"
  "1. make  \n"
  "2. make install\n"
  "3. make doxygen (optional)\n"
  "\n"
  "<b>%s.h</b> will bin installed in <i>%s/include</i>\n"
  "\n"
  "<b>lib%s.a</b> will be installed in <i>%s/lib</i>\n"
  "\n"
  "The code documentation will be installed in <i>%s/share/doc/%s/html</i>.\n"
  "\n"
  "[Back to Table of Contents](#TOC)\n"
  "\n"
  "<a id=\"known-issues-and-limitations\"></a>\n"
  "## Known issues and limitations\n"
  "\n"
  "No known bugs or issues\n"
  "\n"
  "[Back to Table of Contents](#TOC)\n"
  "\n"
  "<a id=\"getting-help\"></a>\n"
  "## Getting help\n"
  "\n"
  "If you encounter any bugs or require additional help, contact [%s](mailto:%s)\n"
  "\n"
  "[Back to Table of Contents](#TOC)\n"
  "\n"
  "## License\n"
  "\n"
  "%s\n"
  "\n"
  "[Back to Table of Contents](#TOC)\n"
  "\n";

/**
 *  @fn void emit_readme(FILE *outfile, char *project_name)
 *
 *  @brief outputs README file
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_readme(FILE *outfile, char *project_name)
{
  char *license_text = NULL;
  char *p;

  license_text = strdup(license_get_text(license_get_type()));
  if (!license_text) license_text = "NO LICENSE";
  else
  {
    p = license_text;
    while (*p)
    {
      if (*p == '/' && *(p + 1) == '*')
      {
        *(p + 1) = *p = ' ';
        ++p;
      }
      else if (*p == '*' && *(p + 1) == '/')
      {
        *(p + 1) = *p = ' ';
        ++p;
      }
      else if (*p == '*')
        *p = ' ';
      ++p;
    }
  }

  fprintf(outfile,
          _readme,
          project_name,
          project_name,
          option_makefile_install_dir(),
          project_name,
          option_makefile_install_dir(),
          option_makefile_install_dir(),
          project_name,
          "CONTACT NAME",
          "CONTACT EMAIL ADDRESS",
          license_text
         );

  free(license_text);
}

