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
 *  @file source-avl.c
 *  @brief add-on for source.c
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>

#include "config.h"

#include "source-avl.h"
#include "options.h"

static void emit_aggregate_avl_new_function(FILE *outfile,
                                            xmlNodePtr node,
                                            char *project,
                                            int indent);
static void emit_aggregate_avl_dup_function(FILE *outfile,
                                            xmlNodePtr node,
                                            char *project,
                                            int indent);
static void emit_aggregate_avl_free_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent);
static void emit_aggregate_avl_insert_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent);
static void emit_aggregate_avl_delete_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent);
static void emit_aggregate_avl_find_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent);
static void emit_aggregate_avl_walk_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent);
static void emit_aggregate_avl_new_node_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent);
static void emit_aggregate_avl_dup_node_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent);
static void emit_aggregate_avl_free_node_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);
static void emit_aggregate_avl_cmp_node_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent);

static void emit_aggregate_avl_new_annotation(FILE *outfile,
                                              xmlNodePtr node,
                                              char *aggregate_name,
                                              char *function_prefix,
                                              int indent);
static void emit_aggregate_avl_dup_annotation(FILE *outfile,
                                              xmlNodePtr node,
                                              char *aggregate_name,
                                              char *function_prefix,
                                              int indent);
static void emit_aggregate_avl_free_annotation(FILE *outfile,
                                               xmlNodePtr node,
                                               char *aggregate_name,
                                               char *function_prefix,
                                               int indent);
static void emit_aggregate_avl_insert_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent);
static void emit_aggregate_avl_delete_annotation(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *aggregate_name,
                                                 char *function_prefix,
                                                 int indent);
static void emit_aggregate_avl_find_annotation(FILE *outfile,
                                               xmlNodePtr node,
                                               char *aggregate_name,
                                               char *function_prefix,
                                               int indent);
static void emit_aggregate_avl_walk_annotation(FILE *outfile,
                                               xmlNodePtr node,
                                               char *aggregate_name,
                                               char *function_prefix,
                                               int indent);
static void emit_aggregate_avl_new_node_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *function_prefix,
                                                   int indent);
static void emit_aggregate_avl_dup_node_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *function_prefix,
                                                   int indent);
static void emit_aggregate_avl_free_node_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);
static void emit_aggregate_avl_cmp_node_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *function_prefix,
                                                   int indent);

  /**
   *  @fn void emit_aggregate_avl_functions(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *project_name)
   *
   *  @brief generates avl C source code from struct or union element in
   *         @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_avl_functions(FILE *outfile,
                                    xmlNodePtr node,
                                    char *project_name)
{
  char *project = NULL;
  char *name = NULL;
  int indent = 0;

  if (!option_gen_avl()) goto exit;

  if (!outfile || !node || !project_name) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  name = strapp(name, "_avl");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " *  Utility functions for %s %s\n", node->name, name);

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

  emit_aggregate_avl_new_function(outfile, node, project, indent);
  emit_aggregate_avl_dup_function(outfile, node, project, indent);
  emit_aggregate_avl_free_function(outfile, node, project, indent);
  emit_aggregate_avl_insert_function(outfile, node, project, indent);
  emit_aggregate_avl_delete_function(outfile, node, project, indent);
  emit_aggregate_avl_find_function(outfile, node, project, indent);
  emit_aggregate_avl_walk_function(outfile, node, project, indent);
  emit_aggregate_avl_new_node_function(outfile, node, project, indent);
  emit_aggregate_avl_dup_node_function(outfile, node, project, indent);
  emit_aggregate_avl_free_node_function(outfile, node, project, indent);
  emit_aggregate_avl_cmp_node_function(outfile, node, project, indent);

exit:
  if (project) free(project);
  if (name) free(name);
}

  /**
   *  @fn void emit_aggregate_avl_new_function(FILE *outfile,
   *                                           xmlNodePtr node,
   *                                           char *project,
   *                                           int indent)
   *
   *  @brief generates C source code to create new avl struct from element
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
  
static void emit_aggregate_avl_new_function(FILE *outfile,
                                            xmlNodePtr node,
                                            char *project,
                                            int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strapp(avl_name, name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_new_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile,
          "%s *%s_new(void)\n",
          avl_name,
          fpre);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *instance = NULL;\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "avl *tree = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance = malloc(sizeof(%s));\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "memset(instance, 0, sizeof(%s));\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "tree = instance->_avl = avl_new();\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (tree)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile,
          "avl_set_new(tree, (avl_new_node)%s_new_node_func);\n",
          avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "avl_set_dup(tree, (avl_dup_node)%s_dup_node_func);\n",
          avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "avl_set_free(tree, (avl_free_node)%s_free_node_func);\n",
          avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "avl_set_cmp(tree, (avl_cmp_node)%s_cmp_node_func);\n",
          avl_name);

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}\n");

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}\n");

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
   *  @fn void emit_aggregate_avl_dup_function(FILE *outfile,
   *                                           xmlNodePtr node,
   *                                           char *project,
   *                                           int indent)
   *
   *  @brief generates C source code to duplicate avl struct from
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
  
static void emit_aggregate_avl_dup_function(FILE *outfile,
                                            xmlNodePtr node,
                                            char *project,
                                            int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_dup_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_dup(%s *instance)\n",
                   avl_name,
                   fpre,
                   avl_name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *new_instance = NULL;\n", avl_name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_instance = malloc(sizeof(%s));\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "memcpy(new_instance, instance, sizeof(%s));\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "new_instance->_avl = avl_dup(instance->_avl);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_instance;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_free_function(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *project,
   *                                            int indent)
   *
   *  @brief generates C source code to free avl struct from element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_avl_free_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_free_annotation(outfile,
                                       node,
                                       avl_name,
                                       fpre2,
                                       indent + 1);

  fprintf(outfile,
          "void %s_free(%s *instance)\n",
           fpre,
           avl_name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->_avl) avl_free(instance->_avl);\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "free(instance);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_insert_function(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project,
   *                                              int indent)
   *
   *  @brief generates C source code to inserta new node into avl struct from
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
  
static void emit_aggregate_avl_insert_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_insert_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_insert(%s *instance, %s *item)\n",
                   fpre,
                   avl_name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_avl || !item) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "avl_insert(instance->_avl, (void *)item);\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_avl_delete_function(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project,
   *                                              int indent)
   *
   *  @brief generates C source code to delete a node from avl struct
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
  
static void emit_aggregate_avl_delete_function(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_delete_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_delete(%s *instance, %s *target)\n",
                   fpre,
                   avl_name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "void *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_avl || !target) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "found = avl_find(instance->_avl, (void *)target);\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (found) avl_delete(instance->_avl, found);\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_find_function(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *project,
   *                                            int indent)
   *
   *  @brief generates C source code to find a node in avl struct
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
  
static void emit_aggregate_avl_find_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_find_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_find(%s *instance, %s *needle)\n",
                   name, 
                   fpre,
                   avl_name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "avl_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_avl || !needle) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "found = avl_find(instance->_avl, (avl_node *)needle);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return (%s *)&((%s_node *)found)->data;\n", name, avl_name);

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_walk_function(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *project,
   *                                            int indent)
   *
   *  @brief generates C source code to walk an entire tree in AVL struct
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
  
static void emit_aggregate_avl_walk_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_walk_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_walk(%s *instance,\n", fpre, avl_name);

  fprintf(outfile, "          avl_order order,\n");

  fprintf(outfile, "          %s_action action)\n", fpre);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_avl || !action) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "avl_walk(instance->_avl, order, (avl_action)action);\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_new_node_function(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *project,
   *                                                int indent)
   *
   *  @brief generates C source code to create a new avl node
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
  
static void emit_aggregate_avl_new_node_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_new_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s_node *%s_new_node_func(void)\n", avl_name, fpre);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s_node *new_node = NULL;\n", avl_name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_node = malloc(sizeof(%s_node));\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_node) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "memset(new_node, 0, sizeof(%s_node));\n", avl_name);

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_node;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_dup_node_function(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *project,
   *                                                int indent)
   *
   *  @brief generates C source code to copy a avl node
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
  
static void emit_aggregate_avl_dup_node_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_dup_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile,
          "%s_node *%s_dup_node_func(%s_node *node)\n",
          avl_name,
          fpre,
          avl_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s_node *new_node = NULL;\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *item = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!node) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_node = %s_new_node_func();\n", avl_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_node) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "item = %s_dup((%s *)&node->data);\n", name, name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!item) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "memcpy(&new_node->data, item, sizeof(%s));\n", name);

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (item) %s_free(item);\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_node;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_free_node_function(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *project,
   *                                                 int indent)
   *
   *  @brief generates C source code to free memory allocated to avl node
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
  
static void emit_aggregate_avl_free_node_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_free_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_free_node_func(%s_node *node)\n", fpre, avl_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!node) goto exit;\n");

  fprintf(outfile, "\n");

  fprintf(outfile,
          "#warning Add any %s specific free() code needed here, then remove "
          "this warning\n",
          name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "free(node);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_cmp_node_function(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *project,
   *                                                int indent)
   *
   *  @brief generates C source code to compare two avl nodes
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
  
static void emit_aggregate_avl_cmp_node_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_avl");

  fpre2 = function_prefix(project, name);

  emit_aggregate_avl_cmp_node_annotation(outfile, node, name, fpre2, indent+1);

  fprintf(outfile,
          "int %s_cmp_node_func(%s_node *a, %s_node *b)\n",
          fpre,
          avl_name,
          avl_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "int rv = 0;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!a || !b) return 0;\n");

  fprintf(outfile, "\n");

  fprintf(outfile,
          "#warning Add or change any %s specific comparison code needed here, "
          "then remove this warning\n",
          name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "rv = memcmp(&a->data, &b->data, sizeof(%s));\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (rv < 0) return -1;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (rv > 0) return 1;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return 0;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_avl_new_annotation(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *aggregate_name,
   *                                             char *function_prefix,
   *                                             int indent)
   *
   *  @brief emits annotation for aggregate avl new function
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
  
static void emit_aggregate_avl_new_annotation(FILE *outfile,
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
              " *  @fn %s_avl *%s_avl_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a new @a %s_avl struct\n",
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
          " *  @return pointer to new @a %s_avl on success, "
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
              " *  %s_avl *%s_avl_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a new %s_avl struct\n",
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
              " *    pointer to new %s_avl on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_avl_dup_annotation(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *aggregate_name,
   *                                             char *function_prefix,
   *                                             int indent)
   *
   *  @brief emits annotation for aggregate avl dup function
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
  
static void emit_aggregate_avl_dup_annotation(FILE *outfile,
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
              " *  @fn %s_avl *%s_avl_dup(%s_avl *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a deep copy of @a %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
          " *  @return pointer to new @a %s_avl on success, "
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
              " *  fn %s_avl *%s_avl_dup(%s *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a deep copy of %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to new %s_avl on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_avl_free_annotation(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *aggregate_name,
   *                                              char *function_prefix,
   *                                              int indent)
   *
   *  @brief emits annotation for aggregate avl free function
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
  
static void emit_aggregate_avl_free_annotation(FILE *outfile,
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
              " *  @fn void %s_avl_free(%s *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief frees all memory allocated to @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s struct\n",
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
              " *  void %s_avl_free(%s *instance)\n",
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
              " *    instance - pointer to %s_avl struct\n",
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
   *  @fn void emit_aggregate_avl_insert_annotation(FILE *outfile,
   *                                                xmlNodePtr node,
    *                                               char *aggregate_name,
    *                                               char *function_prefix,
    *                                               int indent)
   *
   *  @brief emits annotation for aggregate avl add function
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
  
static void emit_aggregate_avl_insert_annotation(FILE *outfile,
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
              " *  @fn void %s_avl_insert(%s_avl *instance, %s *item)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief inserts @p item into @p instance avl\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_avl struct\n",
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
      fprintf(outfile, " *       Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  void %s_avl_insert(%s_avl *instance, %s *item)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  adds item to instance avl\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_avl struct\n",
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
   *  @fn void emit_aggregate_avl_delete_annotation(FILE *outfile,
   *                                                xmlNodePtr node,
   *                                                char *aggregate_name,
   *                                                char *function_prefix,
   *                                                int indent)
   *
   *  @brief emits annotation for aggregate avl delete function
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
  
static void emit_aggregate_avl_delete_annotation(FILE *outfile,
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
              " *  @fn void %s_avl_delete(%s_avl *instance, %s *target)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief deletes @p target from @p instance avl tree\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param target - pointer to @a %s struct\n",
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
              " *  void %s_avl_delete(%s_avl *instance, %s *target)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  removes target from instance avl\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  target - pointer to %s struct\n",
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
   *  @fn void emit_aggregate_avl_find_annotation(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *aggregate_name,
   *                                              char *function_prefix,
   *                                              int indent)
   *
   *  @brief emits annotation for aggregate avl find function
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
  
static void emit_aggregate_avl_find_annotation(FILE *outfile,
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
              " *  @fn %s *%s_avl_find(%s_avl *instance, %s *needle)\n",
              aggregate_name,
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief finds node that matches @p needle in @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param needle - pointer to @a %s struct to look for\n",
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
              " *  %s *%s_avl_find(%s_avl *instance, %s *needle)\n",
              aggregate_name,
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  finds node that matches @p needle in @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    needle - pointer to %s struct to find\n",
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
   *  @fn void emit_aggregate_avl_walk_annotation(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *aggregate_name,
   *                                              char *function_prefix,
   *                                              int indent)
   *
   *  @brief emits annotation for aggregate avl walk function
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
  
static void emit_aggregate_avl_walk_annotation(FILE *outfile,
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
              " *  @fn void %s_avl_walk(%s_avl *instance,\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *            avl_order order,\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *            %s_avl_action action)\n", aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief walks through all items in @p instance calling "
              " @p action\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *         for each node encountered\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_avl struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param order - @a avl_order to user for tree walk\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param action - function to call for each node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @par Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *       Nothing.\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  void %s_avl_walk(%s_avl *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns tail item in %s_avl\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_avl struct\n",
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
   *  @fn void emit_aggregate_avl_new_node_annotation(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *aggregate_name,
   *                                                  char *function_prefix,
   *                                                  int indent)
   *
   *  @brief emits annotation for aggregate avl new node function
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
  
static void emit_aggregate_avl_new_node_annotation(FILE *outfile,
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
              " *  @fn %s_avl_node *%s_avl_new_node_func(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief avl helper function, creates @a %s_avl_node\n",
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
              " *  @return pointer to @a %s_avl_node on success, "
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
              " *  %s_avl_node *%s_avl_new_node_func(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  avl helper function, creates @a %s_avl_node\n",
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
              " *    pointer to %s_avl_node on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_avl_dup_node_annotation(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *aggregate_name,
   *                                                  char *function_prefix,
   *                                                  int indent)
   *
   *  @brief emits annotation for aggregate avl dup node function
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
  
static void emit_aggregate_avl_dup_node_annotation(FILE *outfile,
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
              " *  @fn %s_avl_node *%s_avl_dup_node_func(%s_avl_node *node)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief avl helper function, copies @p node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param node - pointer to @a %s_avl_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s_avl_node on success, "
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
              " *  avl_node *%s_avl_dup_node_func(avl_node *node)\n",
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  avl helper function, copies an node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  node - pointer to avl_node struct\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to avl_node on success, NULL on failure\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_avl_free_node_annotation(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *aggregate_name,
   *                                                   char *function_prefix,
   *                                                   int indent)
   *
   *  @brief emits annotation for aggregate avl free node function
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
  
static void emit_aggregate_avl_free_node_annotation(FILE *outfile,
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
              " *  @fn void %s_avl_free_node_func(%s_avl_node *node)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief avl helper function, frees @p node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param node - pointer to @a %s_avl_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @par Returns\n");

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
              " *  void %s_avl_free_node_func(%s_avl_node *node)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  avl helper function, frees node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  node - pointer to %s_avl_node struct\n",
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
   *  @fn void emit_aggregate_avl_cmp_node_annotation(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *aggregate_name,
   *                                                  char *function_prefix,
   *                                                  int indent)
   *
   *  @brief emits annotation for aggregate avl cmp node function
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
  
static void emit_aggregate_avl_cmp_node_annotation(FILE *outfile,
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
              " *  @fn int %s_avl_cmp_node_func(%s_avl_node *a, "
              "%s_avl_node *b)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief avl helper function, compares @p a to @p b\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param a - pointer to @a %s_avl_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param b - pointer to @a %s_avl_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @return -1 if a<b, 0 if a==b, 1 if a>b\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      fprintf(outfile,
              " *  int %s_avl_cmp_node_func(%s_avl_node *a, %s_avl_node *b)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  avl helper function, compares a to b\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  a - pointer to %s_avl_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  b - pointer to %s_avl_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    -1 if a<b, 0 if a==b, 1 if a>b\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

