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
 *  @file source-array.c
 *  @brief add-on for source.c
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>

#include "config.h"

#include "source-array.h"
#include "options.h"

static void emit_aggregate_array_new_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_array_dup_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_array_free_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent);
static void emit_aggregate_array_get_current_function(FILE *outfile,
                                                      xmlNodePtr node,
                                                      char *project,
                                                      int indent);
static void emit_aggregate_array_add_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_array_remove_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent);
static void emit_aggregate_array_first_function(FILE *outfile,
                                                xmlNodePtr node,
                                                char *project,
                                                int indent);
static void emit_aggregate_array_next_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent);
static void emit_aggregate_array_previous_function(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *project,
                                                   int indent);
static void emit_aggregate_array_last_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent);
static void emit_aggregate_array_current_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);
static void emit_aggregate_array_new_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_array_dup_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_array_free_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent);
static void emit_aggregate_array_get_current_annotation(FILE *outfile,
                                                        xmlNodePtr node,
                                                        char *aggregate_name,
                                                        char *function_prefix,
                                                        int indent);
static void emit_aggregate_array_add_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_array_remove_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *function_prefix,
                                                   int indent);
static void emit_aggregate_array_first_annotation(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *aggregate_name,
                                                  char *function_prefix,
                                                  int indent);
static void emit_aggregate_array_next_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent);
static void emit_aggregate_array_previous_annotation(FILE *outfile,
                                                     xmlNodePtr node,
                                                     char *aggregate_name,
                                                     char *function_prefix,
                                                     int indent);
static void emit_aggregate_array_last_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent);
static void emit_aggregate_array_current_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);

  /**
   *  @fn void emit_aggregate_array_functions(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *project_name)
   *
   *  @brief generates list C source code from struct or union element in
   *         @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_array_functions(FILE *outfile,
                                    xmlNodePtr node,
                                    char *project_name)
{
  char *project = NULL;
  char *name = NULL;
  int indent = 0;

  if (!option_gen_array()) goto exit;

  if (!outfile || !node || !project_name) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  name = strapp(name, "_array");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " *  Utility functions for %s %s\n", node->name, name);

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

  emit_aggregate_array_new_function(outfile, node, project, indent);
  emit_aggregate_array_dup_function(outfile, node, project, indent);
  emit_aggregate_array_free_function(outfile, node, project, indent);
  emit_aggregate_array_get_current_function(outfile, node, project, indent);
  emit_aggregate_array_add_function(outfile, node, project, indent);
  emit_aggregate_array_remove_function(outfile, node, project, indent);
  emit_aggregate_array_first_function(outfile, node, project, indent);
  emit_aggregate_array_next_function(outfile, node, project, indent);
  emit_aggregate_array_previous_function(outfile, node, project, indent);
  emit_aggregate_array_last_function(outfile, node, project, indent);
  emit_aggregate_array_current_function(outfile, node, project, indent);

exit:
  if (project) free(project);
  if (name) free(name);
}

  /**
   *  @fn void emit_aggregate_array_new_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
   *
   *  @brief generates C source code to create new list struct from element
   *         in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_new_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strapp(list_name, name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_new_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile,
          "%s *%s_new(void)\n",
          list_name,
          fpre);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *instance = NULL;\n", list_name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance = malloc(sizeof(%s));\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance) memset(instance, 0, sizeof(%s));\n", list_name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return instance;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_dup_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
   *
   *  @brief generates C source code to duplicate list struct from
   *         element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_dup_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_dup_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_dup(%s *instance)\n",
                   list_name,
                   fpre,
                   list_name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *new_instance = NULL;\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "int i;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_instance = malloc(sizeof(%s));\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "for (i = 0; i < instance->n; i++)\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile,
          "%s_add(new_instance, instance->item[i]);\n",
          fpre);

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_instance;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_free_function(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project,
   *                                              int indent)
   *
   *  @brief generates C source code to free list struct from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_free_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_free_annotation(outfile,
                                       node,
                                       list_name,
                                       fpre2,
                                       indent + 1);

  fprintf(outfile,
          "void %s_free(%s *instance)\n",
           fpre,
           list_name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "int i;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "for (i = 0; i < instance->n; i++)\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile,
          "%s_free(instance->item[i]);\n",
          fpre2);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "free(instance->item);\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "free(instance);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_get_current_function(FILE *outfile,
   *                                                     xmlNodePtr node,
   *                                                     char *project,
   *                                                     int indent)
   *
   *  @brief generates C source code to get value of 'current' field in list
   *         struct from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_get_current_function(FILE *outfile,
                                                      xmlNodePtr node,
                                                      char *project,
                                                      int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_get_current_annotation(outfile,
                                             node,
                                             name,
                                             fpre2,
                                             indent + 1);

  fprintf(outfile, "int %s_get_current(%s *instance)\n",
                   fpre,
                   list_name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "return instance ? instance->current : 0;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_add_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
   *
   *  @brief generates C source code to add a new item to list struct from
   *         from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_add_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_add_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_add(%s *instance, %s *item)\n",
                   fpre,
                   list_name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "void *tmp = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !item) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "tmp = realloc(instance->item,"
          " sizeof(%s *) * (instance->n + 1));\n",
          name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!tmp) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance->item = tmp;\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "instance->item[instance->n] = %s_dup(item);\n",
          fpre2);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "++instance->n;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_array_remove_function(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *project,
   *                                                int indent)
   *
   *  @brief generates C source code to remove an item from list struct from
   *         from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_remove_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_remove_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_remove(%s *instance, int index)\n",
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "int i;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "void *tmp = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) return;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (index < 0) return;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (index >= instance->n) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "%s_free(instance->item[index]);\n",
          fpre2);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "for (i = index; i < instance->n - 1; i++)\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile, "instance->item[i] = instance->item[i + 1];\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "--instance->n;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "tmp = realloc(instance->item,"
          " sizeof(%s *) * instance->n);\n",
          name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!tmp) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance->item = tmp;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_first_function(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *project,
   *                                                int indent)
   *
   *  @brief generates C source code to return the first item from list struct
   *         from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_first_function(FILE *outfile,
                                                xmlNodePtr node,
                                                char *project,
                                                int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_first_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_first(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *item = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance->n) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance->current = 0;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "item = instance->item[0];\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return item;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_next_function(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project,
   *                                              int indent)
   *
   *  @brief generates C source code to return the next item from list struct
   *         from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_next_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_next_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_next(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *item = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance->n) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->current < 0) instance->current = 0;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "else if (instance->current >= instance->n)"
                   " instance->current = instance->n - 1;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "++instance->current;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->current >= instance->n)\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile, "instance->current = 0;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "else\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile, "item = instance->item[instance->current];\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return item;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_previous_function(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *project,
   *                                                  int indent)
   *
   *  @brief generates C source code to return the previous item from list
   *         struct from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_previous_function(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *project,
                                                   int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_previous_annotation(outfile,
                                          node, 
                                          name, 
                                          fpre2, 
                                          indent + 1);

  fprintf(outfile, "%s *%s_previous(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *item = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance->n) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->current < 0) instance->current = 0;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "else if (instance->current >= instance->n)"
                   " instance->current = instance->n - 1;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "--instance->current;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->current < 0)\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile, "instance->current = instance->n - 1;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "else\n");

  emit_indent(outfile, indent + 1);
  fprintf(outfile, "item = instance->item[instance->current];\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return item;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_last_function(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project,
   *                                              int indent)
   *
   *  @brief generates C source code to return the last item from list struct
   *         from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_last_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_last_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_last(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *item = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance->n) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance->current = instance->n - 1;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "item = instance->item[instance->current];\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return item;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_current_function(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *project,
   *                                                 int indent)
   *
   *  @brief generates C source code to return the current item from array
   *         struct, from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_current_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  list_name = strdup(name);
  list_name = strapp(list_name, "_array");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_array");

  fpre2 = function_prefix(project, name);

  emit_aggregate_array_current_annotation(outfile,
                                         node,
                                         name,
                                         fpre2,
                                         indent + 1);

  fprintf(outfile, "%s *%s_current(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *item = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance->n) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->current < 0) instance->current = 0;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->current >= instance->n)"
                   " instance->current = instance->n - 1;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "item = instance->item[instance->current];\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return item;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_array_new_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
   *
   *  @brief emits annotation for aggregate list new function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_new_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s_array *%s_array_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a new @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @par Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    None.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
          " *  @return pointer to new @a %s_array on success, "
          "NULL on failure\n",
          aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s_array *%s_array_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a new %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    None.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to new %s_array on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_dup_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
   *
   *  @brief emits annotation for aggregate list dup function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_dup_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s_array *%s_array_dup(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a deep copy of @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
          " *  @return pointer to new @a %s_array on success, "
          "NULL on failure\n",
          aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  fn %s_array *%s_array_dup(%s *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a deep copy of %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to new %s_array on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_free_annotation(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *aggregate_name,
   *                                                char *function_prefix,
   *                                                int indent)
   *
   *  @brief emits annotation for aggregate list free function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_free_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn void %s_array_free(%s *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief frees all memory allocated to @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  void %s_array_free(%s *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  frees all memory allocated to instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_get_current_annotation(FILE *outfile,
   *                                                     xmlNodePtr node,
   *                                                     char *aggregate_name,
   *                                                     char *function_prefix,
   *                                                     int indent)
   *
   *  @brief emits annotation for aggregate list get_current function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_get_current_annotation(FILE *outfile,
                                                        xmlNodePtr node,
                                                        char *aggregate_name,
                                                        char *function_prefix,
                                                        int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn int %s_array_get_current(%s_array *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns current index value of @p instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return current index value of @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  int %s_array_get_current(%s_array *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns current index value of instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    return current index value of @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_add_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
   *
   *  @brief emits annotation for aggregate list add function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_add_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn void %s_array_add(%s_array *instance, %s *item)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief adds @p item to @p instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param item - pointer to @a %s struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @par Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  void %s_array_add(%s_array *instance, %s *item)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  adds item to instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    item - pointer to %s struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_remove_annotation(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *aggregate_name,
   *                                                  char *function_prefix,
   *                                                  int indent)
   *
   *  @brief emits annotation for aggregate list remove function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_remove_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *function_prefix,
                                                   int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn void %s_array_remove(%s_array *instance, int index)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief removes item at @p index from @p instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param index - index into instance->item array\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @par Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  void %s_array_remove(%s_array *instance, int index)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  removes item at index from instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    index - index into instance->item array\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_first_annotation(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *aggregate_name,
   *                                                 char *function_prefix,
   *                                                 int indent)
   *
   *  @brief emits annotation for aggregate list first function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function  name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_first_annotation(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *aggregate_name,
                                                  char *function_prefix,
                                                  int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s *%s_array_first(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns first item in @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s *%s_array_first(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns first item in %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_next_annotation(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *aggregate_name,
   *                                                char *function_prefix,
   *                                                int indent)
   *
   *  @brief emits annotation for aggregate list next function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_next_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s *%s_array_next(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns next item in @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s *%s_array_next(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns next item in %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_previous_annotation(FILE *outfile,
   *                                                    xmlNodePtr node,
   *                                                    char *aggregate_name,
   *                                                    char *function_prefix,
   *                                                    int indent)
   *
   *  @brief emits annotation for aggregate list previous function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_previous_annotation(FILE *outfile,
                                                     xmlNodePtr node,
                                                     char *aggregate_name,
                                                     char *function_prefix,
                                                     int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s *%s_array_previous(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns previous item in @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s *%s_array_previous(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns previous item in %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_last_annotation(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *aggregate_name,
   *                                                char *function_prefix,
   *                                                int indent)
   *
   *  @brief emits annotation for aggregate list last function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_last_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s *%s_array_last(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns last item in @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s *%s_array_last(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns last item in %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_array_current_annotation(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *aggregate_name,
   *                                                   char *function_prefix,
   *                                                   int indent)
   *
   *  @brief emits annotation for aggregate list current function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing aggregate name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_array_current_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent)
{
  if (!outfile || !node || !aggregate_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s *%s_array_current(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns current item in @a %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s *%s_array_current(%s_array *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns current item in %s_array\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_array struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to %s on success, NULL on failure\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

