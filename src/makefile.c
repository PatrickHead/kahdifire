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
 *  @file makefile.c
 *  @brief Converts C declarations to makefile
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is makefile
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

#include "makefile.h"
#include "options.h"

  /*  Module specific function prototypes  */

static void emit_blank(FILE *outfile);
static void emit_options(FILE *outfile);
static void emit_install_dir(FILE *outfile);
static void emit_all(FILE *outfile, char *project_name);
static void emit_doxygen(FILE *outfile, char *project_name);
static void emit_library(FILE *outfile, char *project_name);
static void emit_object(FILE *outfile, char *project_name);
static void emit_clean(FILE *outfile, char *project_name);
static void emit_install(FILE *outfile, char *project_name);
static void emit_uninstall(FILE *outfile, char *project_name);

/**
 *  @fn void gen_makefile(xmlDocPtr doc, char *base_name)
 *
 *  @brief generates makefile from enum, struct and union declarations
 *
 *  @param doc - xmlDocPtr containing declaration metadata
 *  @param base_name - basic name of project for output files
 *
 *  @par Returns
 *  Nothing.
 */

void gen_makefile(xmlDocPtr doc, char *base_name)
{
  xmlNodePtr root;
  FILE *outfile = NULL;
  char *tmp;
  char *base_dir = NULL;
  char *outfile_name = NULL;
  char *project_name = NULL;

  if (!doc || !base_name) goto exit;

  if (!option_gen_makefile()) goto exit;

  root = xmlDocGetRootElement(doc);
  if (!root) goto exit;

  if (!root->children) goto exit;

  if (strcmp((char *)root->name, "c-decls")) goto exit;

  tmp = strdup(base_name);
  if (!tmp) goto exit;

  base_dir = dirname(tmp);

  outfile_name = malloc(strlen(base_dir) + strlen("Makefile") + 2);
  if (!outfile_name) goto exit;

  sprintf(outfile_name, "%s/Makefile", base_dir);

  outfile = fopen(outfile_name, "w");
  if (!outfile) goto exit;

  project_name = get_project_name(base_name);
  if (!project_name) goto exit;

  emit_options(outfile);
  emit_install_dir(outfile);
  emit_blank(outfile);
  emit_all(outfile, project_name);
  emit_blank(outfile);
  emit_doxygen(outfile, project_name);
  emit_library(outfile, project_name);
  emit_blank(outfile);
  emit_object(outfile, project_name);
  emit_blank(outfile);
  emit_clean(outfile, project_name);
  emit_blank(outfile);
  emit_install(outfile, project_name);
  emit_blank(outfile);
  emit_uninstall(outfile, project_name);

exit:
  if (outfile) fclose(outfile);
  if (outfile_name) free(outfile_name);
  if (project_name) free(project_name);
  if (base_dir) free(base_dir);
}

/**
 *  @fn void emit_blank(FILE *outfile)
 *
 *  @brief adds a blank line to makefile
 *
 *  @param outfile - FILE * open for writing
 *
 *  @par Returns
 *  Nothing.
 */

void emit_blank(FILE *outfile)
{
  fprintf(outfile, "\n");
}

/**
 *  @fn void emit_options(FILE *outfile)
 *
 *  @brief adds 'CC = compiler'
 *         and  'COPTS = compiler options' line to makefile
 *
 *  @param outfile - FILE * open for writing
 *
 *  @par Returns
 *  Nothing.
 */

void emit_options(FILE *outfile)
{
  fprintf(outfile, "CC = %s\n", option_makefile_cc());
  fprintf(outfile, "COPTS = %s\n", option_makefile_copts());
}

/**
 *  @fn void emit_install_dir(FILE *outfile)
 *
 *  @brief adds 'INSTALL_DIR = /usr/local' type line to makefile
 *
 *  @param outfile - FILE * open for writing
 *
 *  @par Returns
 *  Nothing.
 */

void emit_install_dir(FILE *outfile)
{
  fprintf(outfile, "INSTALL_DIR = %s\n", option_makefile_install_dir());
}

/**
 *  @fn void emit_all(FILE *outfile, char *project_name);
 *
 *  @brief adds default (all:) target rule
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_all(FILE *outfile, char *project_name)
{
  fprintf(outfile, "all: lib%s.a\n", project_name);
}

/**
 *  @fn void emit_doxygen(FILE *outfile, char *project_name)
 *
 *  @brief adds target rule to create Doxygen documentation
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_doxygen(FILE *outfile, char *project_name)
{
  if (option_annotation() != annotation_type_doxygen) return;

  fprintf(outfile, "doxygen: Doxygen.%s\n", project_name);
	fprintf(outfile, "\t@echo Creating Doxygen documents\n");
	fprintf(outfile, "\t@doxygen Doxygen.%s\n", project_name);
  fprintf(outfile, "\t@touch doxygen\n");
  fprintf(outfile, "\n");
}

/**
 *  @fn void emit_library(FILE *outfile, char *project_name)
 *
 *  @brief adds target rule to create a static (.a) library
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_library(FILE *outfile, char *project_name)
{
  fprintf(outfile, "lib%s.a: %s.o\n", project_name, project_name);
  fprintf(outfile, "\t@echo Creating lib%s.a\n", project_name);
  fprintf(outfile,
          "\t@ar r lib%s.a %s.o 2> /dev/null\n",
          project_name,
          project_name);
}

/**
 *  @fn void emit_object(FILE *outfile, char *project_name)
 *
 *  @brief adds target rule to object (.o)
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_object(FILE *outfile, char *project_name)
{
  fprintf(outfile, "%s.o: %s.c %s.h\n",
          project_name,
          project_name,
          project_name);
  fprintf(outfile, "\t@echo Creating %s.o\n", project_name);
  fprintf(outfile, "\t@$(CC) $(COPTS) -c %s.c\n", project_name);
}

/**
 *  @fn void emit_clean(FILE *outfile, char *project_name)
 *
 *  @brief adds target rule to clean build directory
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_clean(FILE *outfile, char *project_name)
{
  fprintf(outfile, "clean:\n");
	fprintf(outfile, "\t@rm -f %s.o lib%s.a\n",
          project_name,
          project_name);
	fprintf(outfile, "\t@rm -rf doxygen\n");
}

/**
 *  @fn void emit_install(FILE *outfile, char *project_name)
 *
 *  @brief adds target rule to install library and header on system
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_install(FILE *outfile, char *project_name)
{
  fprintf(outfile, "install:\n");
  fprintf(outfile,
          "\t@echo Installing lib%s.a and %s.h to $(INSTALL_DIR)/lib\n",
          project_name,
          project_name);
  fprintf(outfile, "\t@cp lib%s.a $(INSTALL_DIR)/lib\n", project_name);
  fprintf(outfile, "\t@cp %s.h $(INSTALL_DIR)/include\n", project_name);
}

/**
 *  @fn void emit_uninstall(FILE *outfile, char *project_name)
 *
 *  @brief adds target rule to uninstall library and header from system
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_uninstall(FILE *outfile, char *project_name)
{
  fprintf(outfile, "uninstall:\n");
  fprintf(outfile,
          "\t@echo Uninstalling lib%s.a and %s.h from $(INSTALL_DIR)/lib\n",
          project_name,
          project_name);
  fprintf(outfile,
          "\t@rm $(INSTALL_DIR)/lib/lib%s.a\n",
          project_name);
  fprintf(outfile,
          "\t@rm $(INSTALL_DIR)/include/%s.h\n",
          project_name);
}

