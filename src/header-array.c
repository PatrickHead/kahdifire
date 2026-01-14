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
 *  @file header-array.c
 *  @brief array add-on to header.c
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>

#include "config.h"

#include "header-array.h"
#include "options.h"

static void emit_aggregate_array_annotation(FILE *outfile,
                                            xmlNodePtr node,
                                            char *aggregate_name,
                                            char *array_name,
                                            int indent);

  /**
   *  @fn void emit_aggregate_array(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits array struct for struct or union from @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_array(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *array_name = NULL;
  char *field = NULL;
  int len;
  int is_doxygen = 0;

  if (!option_gen_array()) goto exit;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen: is_doxygen = 1; break;
    default: is_doxygen = 0; break;
  }

  name = get_attribute(node, "name");
  if (!name) goto exit;

  array_name = strapp(array_name, name);
  array_name = strapp(array_name, "_array");

  emit_aggregate_array_annotation(outfile, node, name, array_name, indent + 1);

  emit_indent(outfile, indent);
  fprintf(outfile, "struct %s\n", array_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  len = strlen(name) + 10;
  if (len < 16) len = 16;

  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s/*%s  number of items in array         */\n",
          len,
          len,
          "int n;",
          is_doxygen ? "*<" : "");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s/*%s  index of current item in array   */\n",
          len,
          len,
          "int current;",
          is_doxygen ? "*<" : "");

  field = strdup(name);
  field = strapp(field, " **item;");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s/*%s  array of items                  */\n",
          len,
          len,
          field,
          is_doxygen ? "*<" : "");

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}");

exit:
  if (name) free(name);
  if (array_name) free(array_name);
  if (field) free(field);
}

  /**
   *  @fn void emit_aggregate_array_function_prototypes(FILE *outfile,
   *                                                    xmlNodePtr node,
   *                                                    char *project_name)
   *
   *  @brief emits utility array function prototypes for struct or union in
   *         @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_array_function_prototypes(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project_name)
{
  char *name = NULL;
  char *project = NULL;
  char *array_name = NULL;
  char *function_prefix = NULL;

  if (!option_gen_array()) goto exit;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  project = strdup(project_name);
  if (!project) goto exit;

  str_lower(project);

  array_name = strdup(name);
  array_name = strapp(array_name, "_array");

  if (strcmp(project, name))
  {
    function_prefix = strapp(function_prefix, project);
    function_prefix = strapp(function_prefix, "_");
    function_prefix = strapp(function_prefix, array_name);
  }
  else
    function_prefix = strdup(array_name);

  emit_indent(outfile, 1);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, 1);
  fprintf(outfile, " *  Utility functions for struct %s\n", array_name);

  emit_indent(outfile, 1);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

    // new, dup, and free

  fprintf(outfile, "%s *%s_new(void);\n", array_name, function_prefix);
  fprintf(outfile,
          "%s *%s_dup(%s *instance);\n",
          array_name,
          function_prefix,
          array_name);
  fprintf(outfile,
          "void %s_free(%s *instance);\n",
          function_prefix,
          array_name);

    // setters and getters

  fprintf(outfile,
          "int %s_get_current(%s *instance);\n",
          function_prefix,
          array_name);

    // add or remove item

  fprintf(outfile,
          "void %s_add(%s *instance, %s *item);\n",
          function_prefix,
          array_name,
          name);
  fprintf(outfile,
          "void %s_remove(%s *instance, int index);\n",
          function_prefix,
          array_name);

    // array iteration functions:
    //   first, next, previous, last, current

  fprintf(outfile,
          "%s *%s_first(%s *instance);\n",
          name,
          function_prefix,
          array_name);
  fprintf(outfile,
          "%s *%s_next(%s *instance);\n",
          name,
          function_prefix,
          array_name);
  fprintf(outfile,
          "%s *%s_previous(%s *instance);\n",
          name,
          function_prefix,
          array_name);
  fprintf(outfile,
          "%s *%s_last(%s *instance);\n",
          name,
          function_prefix,
          array_name);
  fprintf(outfile,
          "%s *%s_current(%s *instance);\n",
          name,
          function_prefix,
          array_name);

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (project) free(project);
  if (array_name) free(array_name);
  if (function_prefix) free(function_prefix);
}

  /**
   *  @fn void emit_aggregate_array_annotation(FILE *outfile,
   *                                           xmlNodePtr node,
   *                                           char *aggregate_name,
   *                                           char *array_name,
   *                                           int indent)
   *
   *  @brief emits annotation for a dynamic array of structs or unions
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing typedef name of base aggregate
   *  @param array_name - string containing typedef array
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_annotation(FILE *outfile,
                                            xmlNodePtr node,
                                            char *aggregate_name,
                                            char *array_name,
                                            int indent)
{
  if (!outfile || !node || !aggregate_name || !array_name) goto exit;
  if (!node->name) goto exit;

  if (!option_annotation()) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen:
      emit_indent(outfile, indent);
      fprintf(outfile, "/**\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @struct %s\n", array_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief struct used to manage a dynamic array of @a %s %ss\n",
              aggregate_name,
              node->name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  struct used to manage a dynamic array of @a %s %ss\n",
              aggregate_name,
              node->name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

