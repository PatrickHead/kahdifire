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
 *  @file source-list.c
 *  @brief add-on for source.c
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>

#include "config.h"

#include "source-list.h"
#include "options.h"

static void emit_aggregate_list_new_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent);
static void emit_aggregate_list_dup_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent);
static void emit_aggregate_list_free_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_list_add_function(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project,
                                             int indent);
static void emit_aggregate_list_remove_function(FILE *outfile,
                                                xmlNodePtr node,
                                                char *project,
                                                int indent);
static void emit_aggregate_list_head_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_list_tail_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_list_current_function(FILE *outfile,
                                                 xmlNodePtr node,
                                                 char *project,
                                                 int indent);
static void emit_aggregate_list_previous_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);
static void emit_aggregate_list_next_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_list_find_function(FILE *outfile,
                                              xmlNodePtr node,
                                              char *project,
                                              int indent);
static void emit_aggregate_list_new_node_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);
static void emit_aggregate_list_dup_node_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);
static void emit_aggregate_list_free_node_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);
static void emit_aggregate_list_cmp_node_function(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *project,
                                                  int indent);

static void emit_aggregate_list_new_annotation(FILE *outfile,
                                               xmlNodePtr node,
                                               char *aggregate_name,
                                               char *function_prefix,
                                               int indent);
static void emit_aggregate_list_dup_annotation(FILE *outfile,
                                               xmlNodePtr node,
                                               char *aggregate_name,
                                               char *function_prefix,
                                               int indent);
static void emit_aggregate_list_free_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_list_add_annotation(FILE *outfile,
                                               xmlNodePtr node,
                                               char *aggregate_name,
                                               char *function_prefix,
                                               int indent);
static void emit_aggregate_list_remove_annotation(FILE *outfile,
                                                  xmlNodePtr node,
                                                  char *aggregate_name,
                                                  char *function_prefix,
                                                  int indent);
static void emit_aggregate_list_head_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_list_tail_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_list_current_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *function_prefix,
                                                   int indent);
static void emit_aggregate_list_previous_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);
static void emit_aggregate_list_next_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_list_find_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *function_prefix,
                                                int indent);
static void emit_aggregate_list_new_node_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);
static void emit_aggregate_list_dup_node_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);
static void emit_aggregate_list_free_node_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);
static void emit_aggregate_list_cmp_node_annotation(FILE *outfile,
                                                    xmlNodePtr node,
                                                    char *aggregate_name,
                                                    char *function_prefix,
                                                    int indent);

  /**
   *  @fn void emit_aggregate_list_functions(FILE *outfile,
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
  
void emit_aggregate_list_functions(FILE *outfile,
                                    xmlNodePtr node,
                                    char *project_name)
{
  char *project = NULL;
  char *name = NULL;
  int indent = 0;

  if (!option_gen_list()) goto exit;

  if (!outfile || !node || !project_name) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  name = strapp(name, "_list");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " *  Utility functions for %s %s\n", node->name, name);

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

  emit_aggregate_list_new_function(outfile, node, project, indent);
  emit_aggregate_list_dup_function(outfile, node, project, indent);
  emit_aggregate_list_free_function(outfile, node, project, indent);
  emit_aggregate_list_add_function(outfile, node, project, indent);
  emit_aggregate_list_remove_function(outfile, node, project, indent);
  emit_aggregate_list_head_function(outfile, node, project, indent);
  emit_aggregate_list_tail_function(outfile, node, project, indent);
  emit_aggregate_list_current_function(outfile, node, project, indent);
  emit_aggregate_list_previous_function(outfile, node, project, indent);
  emit_aggregate_list_next_function(outfile, node, project, indent);
  emit_aggregate_list_find_function(outfile, node, project, indent);
  emit_aggregate_list_new_node_function(outfile, node, project, indent);
  emit_aggregate_list_dup_node_function(outfile, node, project, indent);
  emit_aggregate_list_free_node_function(outfile, node, project, indent);
  emit_aggregate_list_cmp_node_function(outfile, node, project, indent);

exit:
  if (project) free(project);
  if (name) free(name);
}

  /**
   *  @fn void emit_aggregate_list_new_function(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *project,
   *                                            int indent)
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
  
static void emit_aggregate_list_new_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_new_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile,
          "%s *%s_new(void)\n",
          list_name,
          fpre);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *instance = NULL;\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "llist *ll = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance = malloc(sizeof(%s));\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "memset(instance, 0, sizeof(%s));\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "ll = instance->_llist = llist_new();\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (ll)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile,
          "llist_set_new(ll, (llist_new_node)%s_new_node_func);\n",
          list_name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "llist_set_dup(ll, (llist_dup_node)%s_dup_node_func);\n",
          list_name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "llist_set_free(ll, (llist_free_node)%s_free_node_func);\n",
          list_name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "llist_set_cmp(ll, (llist_cmp_node)%s_cmp_node_func);\n",
          list_name);

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
   *  @fn void emit_aggregate_list_dup_function(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *project,
   *                                            int indent)
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
  
static void emit_aggregate_list_dup_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_dup_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_dup(%s *instance)\n",
                   list_name,
                   fpre,
                   list_name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *new_instance = NULL;\n", list_name);

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
  fprintf(outfile, "memcpy(new_instance, instance, sizeof(%s));\n", list_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "new_instance->_llist = llist_dup(instance->_llist);\n");

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
   *  @fn void emit_aggregate_list_free_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
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
  
static void emit_aggregate_list_free_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_free_annotation(outfile,
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
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance->_llist) llist_free(instance->_llist);\n");

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
   *  @fn void emit_aggregate_list_add_function(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *project,
   *                                            int indent)
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
  
static void emit_aggregate_list_add_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_add_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_add(%s *instance,\n"
                   "  llist_position position,\n"
                   "  %s *where,\n"
                   "  %s *item)\n",
                   fpre,
                   list_name,
                   name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist || !item) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "llist_add(instance->_llist, position, "
          "(void *)where, (void *)item);\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_list_remove_function(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *project,
   *                                               int indent)
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
  
static void emit_aggregate_list_remove_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_remove_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_remove(%s *instance, %s *item)\n",
                   fpre,
                   list_name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "void *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist || !item) return;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "found = llist_find_payload(instance->_llist, (void *)item);\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (found) llist_remove(instance->_llist, found);\n");

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
   *  @fn void emit_aggregate_list_head_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
   *
   *  @brief generates C source code to return the head item from list
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
  
static void emit_aggregate_list_head_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_head_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_head(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "found = llist_head(instance->_llist);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return found ? (%s *)found->payload : NULL;\n", name);

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
   *  @fn void emit_aggregate_list_tail_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
   *
   *  @brief generates C source code to return the tail item from list
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
  
static void emit_aggregate_list_tail_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_tail_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_tail(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "found = llist_tail(instance->_llist);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return found ? (%s *)found->payload : NULL;\n", name);

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
   *  @fn void emit_aggregate_list_current_function(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *project,
   *                                                 int indent)
   *
   *  @brief generates C source code to return the current item from list
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
  
static void emit_aggregate_list_current_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_current_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_current(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "found = llist_current(instance->_llist);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return found ? (%s *)found->payload : NULL;\n", name);

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
   *  @fn void emit_aggregate_list_previous_function(FILE *outfile,
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
  
static void emit_aggregate_list_previous_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_previous_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_previous(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "found = llist_previous(instance->_llist);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return found ? (%s *)found->payload : NULL;\n", name);

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
   *  @fn void emit_aggregate_list_next_function(FILE *outfile,
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
  
static void emit_aggregate_list_next_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_next_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_next(%s *instance)\n",
                   name, 
                   fpre,
                   list_name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance || !instance->_llist) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "found = llist_next(instance->_llist);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return found ? (%s *)found->payload : NULL;\n", name);

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
   *  @fn void emit_aggregate_list_find_function(FILE *outfile,
   *                                             xmlNodePtr node,
   *                                             char *project,
   *                                             int indent)
   *
   *  @brief generates C source code to find an item in list
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
  
static void emit_aggregate_list_find_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_find_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s *%s_find(%s *instance, %s *needle)\n",
                   name, 
                   fpre,
                   list_name,
                   name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *search = NULL;\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "llist_node *found = NULL;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance && instance->_llist && needle)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "search = llist_node_new((void *)needle);\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (search)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "found = llist_find(instance->_llist, search);\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "free(search);\n");

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}\n");

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return found ? (%s *)found->payload : NULL;\n", name);

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
   *  @fn void emit_aggregate_list_new_node_function(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *project,
   *                                                 int indent)
   *
   *  @brief generates C source code to create a new list node
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
  
static void emit_aggregate_list_new_node_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_new_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "%s_list_node *%s_new_node_func(void)\n", name, fpre);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s_list_node *new_node = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_node = malloc(sizeof(%s_list_node));\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile,
          "if (new_node) memset(new_node, 0, sizeof(%s_list_node));\n",
          name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_node;\n");

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
   *  @fn void emit_aggregate_list_dup_node_function(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *project,
   *                                                 int indent)
   *
   *  @brief generates C source code to copy a list node
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
  
static void emit_aggregate_list_dup_node_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_dup_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile,
          "%s_list_node *%s_dup_node_func(%s_list_node *node)\n",
          name,
          fpre,
          name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s_list_node *new_node = NULL;\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *new_data = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!node) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_node = %s_list_new_node_func();\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_node) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "memcpy(new_node, node, sizeof(%s_list_node));\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "new_data = %s_dup(&new_node->data);\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_data) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "memcpy(&new_node->data, new_data, sizeof(%s));\n", name);

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (new_data) %s_free(new_data);\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_node;\n");

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
   *  @fn void emit_aggregate_list_free_node_function(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *project,
   *                                                  int indent)
   *
   *  @brief generates C source code to free memory allocated to list node
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
  
static void emit_aggregate_list_free_node_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_free_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile, "void %s_free_node_func(%s_list_node *node)\n", fpre, name);

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
  if (list_name) free(list_name);
  if (fpre) free(fpre);
  if (fpre2) free(fpre2);
}

  /**
   *  @fn void emit_aggregate_list_cmp_node_function(FILE *outfile,
   *                                                 xmlNodePtr node,
   *                                                 char *project,
   *                                                 int indent)
   *
   *  @brief generates C source code to compare two list nodes
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
  
static void emit_aggregate_list_cmp_node_function(FILE *outfile,
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
  list_name = strapp(list_name, "_list");

  fpre = function_prefix(project, name);
  fpre = strapp(fpre, "_list");

  fpre2 = function_prefix(project, name);

  emit_aggregate_list_cmp_node_annotation(outfile, node, name, fpre2, indent + 1);

  fprintf(outfile,
          "int %s_cmp_node_func(%s_list_node *a, %s_list_node *b)\n",
          fpre,
          name,
          name);

  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!a || !b) return 0;\n");

  fprintf(outfile, "\n");

  fprintf(outfile,
          "#warning Add any %s specific comparison code needed here, then "
          "remove this warning\n",
          name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile,
          "return memcmp(&a->data, &b->data, sizeof(%s));\n",
          name);

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
   *  @fn void emit_aggregate_list_new_annotation(FILE *outfile,
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
  
static void emit_aggregate_list_new_annotation(FILE *outfile,
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
              " *  @fn %s_list *%s_list_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a new @a %s_list struct\n",
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
          " *  @return pointer to new @a %s_list on success, "
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
              " *  %s_list *%s_list_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a new %s_list struct\n",
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
              " *    pointer to new %s_list on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_list_dup_annotation(FILE *outfile,
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
  
static void emit_aggregate_list_dup_annotation(FILE *outfile,
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
              " *  @fn %s_list *%s_list_dup(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a deep copy of @a %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
          " *  @return pointer to new @a %s_list on success, "
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
              " *  fn %s_list *%s_list_dup(%s *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a deep copy of %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to new %s_list on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_list_free_annotation(FILE *outfile,
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
  
static void emit_aggregate_list_free_annotation(FILE *outfile,
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
              " *  @fn void %s_list_free(%s *instance)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief frees all memory allocated to @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  void %s_list_free(%s *instance)\n",
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
              " *    instance - pointer to %s_list struct\n",
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
   *  @fn void emit_aggregate_list_add_annotation(FILE *outfile,
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
  
static void emit_aggregate_list_add_annotation(FILE *outfile,
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
              " *  @fn void %s_list_add(%s_list *instance,\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *        llist_position position,\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *        %s *where,\n", aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *        %s *item)\n", aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief adds @p item to @p instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @param position - @a llist_position value\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param where - pointer to @a %s struct\n",
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
              " *  void %s_list_add(%s_list *instance,\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *     llist_position position,\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *     %s *where,\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *     %s *item)\n",
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
              " *    instance - pointer to %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *    position - llist_position value\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    where - pointer to %s struct\n",
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
   *  @fn void emit_aggregate_list_remove_annotation(FILE *outfile,
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
  
static void emit_aggregate_list_remove_annotation(FILE *outfile,
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
              " *  @fn void %s_list_remove(%s_list *instance, %s *item)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief removes @p item from @p instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  void %s_list_remove(%s_list *instance, %s *item)\n",
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  removes item from instance list\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  item - pointer to %s struct\n",
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
   *  @fn void emit_aggregate_list_head_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
   *
   *  @brief emits annotation for aggregate list head function
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
  
static void emit_aggregate_list_head_annotation(FILE *outfile,
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
              " *  @fn %s *%s_list_head(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns head item in @a %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  %s *%s_list_head(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns head item in %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
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
   *  @fn void emit_aggregate_list_tail_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
   *
   *  @brief emits annotation for aggregate list tail function
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
  
static void emit_aggregate_list_tail_annotation(FILE *outfile,
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
              " *  @fn %s *%s_list_tail(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns tail item in @a %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  %s *%s_list_tail(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns tail item in %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
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
   *  @fn void emit_aggregate_list_current_annotation(FILE *outfile,
   *                                                  xmlNodePtr node,
   *                                                  char *aggregate_name,
   *                                                  char *function_prefix,
   *                                                  int indent)
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
  
static void emit_aggregate_list_current_annotation(FILE *outfile,
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
              " *  @fn %s *%s_list_current(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns current item in @a %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  %s *%s_list_current(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns current item in %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
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
   *  @fn void emit_aggregate_list_previous_annotation(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *aggregate_name,
   *                                                   char *function_prefix,
   *                                                   int indent)
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
  
static void emit_aggregate_list_previous_annotation(FILE *outfile,
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
              " *  @fn %s *%s_list_previous(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns previous item in @a %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  %s *%s_list_previous(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns previous item in %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
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
   *  @fn void emit_aggregate_list_next_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
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
  
static void emit_aggregate_list_next_annotation(FILE *outfile,
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
              " *  @fn %s *%s_list_next(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns next item in @a %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
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
              " *  %s *%s_list_next(%s_list *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  returns next item in %s_list\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
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
   *  @fn void emit_aggregate_list_find_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *function_prefix,
   *                                               int indent)
   *
   *  @brief emits annotation for aggregate list find function
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
  
static void emit_aggregate_list_find_annotation(FILE *outfile,
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
              " *  @fn %s *%s_list_find(%s_list *instance, %s *needle)\n",
              aggregate_name,
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief searches @a %s_list for node with @p needle value\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param needle - pointer to @a %s struct\n",
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
              " *  %s *%s_list_find(%s_list *instance, %s *needle)\n",
              aggregate_name,
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      fprintf(outfile,
              " *  searches %s_list for node with needle value\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s_list struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  needle - pointer to %s struct\n",
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
   *  @fn void emit_aggregate_list_new_node_annotation(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *aggregate_name,
   *                                                   char *function_prefix,
   *                                                   int indent)
   *
   *  @brief emits annotation for aggregate list new node function
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
  
static void emit_aggregate_list_new_node_annotation(FILE *outfile,
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
              " *  @fn %s__node *%s_list_new_node_func(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief llist helper function, creates an @a %s_list_node\n",
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
              " *  @return pointer to @a %s_list_node on success, "
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
              " *  %s_list_node *%s_list_new_node_func(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  llist helper function, creates an @a %s_list_node\n",
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
              " *    pointer to %s_list_node on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_list_dup_node_annotation(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *aggregate_name,
   *                                                   char *function_prefix,
   *                                                   int indent)
   *
   *  @brief emits annotation for aggregate list dup node function
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
  
static void emit_aggregate_list_dup_node_annotation(FILE *outfile,
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
              " *  @fn %s_list_node *%s_list_dup_node_func"
              "(%s_list_node *node)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief llist helper function, copies an @p node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param node - pointer to @a %s_list_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to @a %s_list_node on success, "
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
              " *  %s_list_node *%s_list_dup_node_func(%s_list_node *node)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  llist helper function, copies an node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  node - pointer to %s_list_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to %s_list_node on success, NULL on failure\n",
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
   *  @fn void emit_aggregate_list_free_node_annotation(FILE *outfile,
   *                                                    xmlNodePtr node,
   *                                                    char *aggregate_name,
   *                                                    char *function_prefix,
   *                                                    int indent)
   *
   *  @brief emits annotation for aggregate list free node function
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
  
static void emit_aggregate_list_free_node_annotation(FILE *outfile,
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
              " *  @fn void %s_list_free_node_func (%s_list_node *node)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief llist helper function, frees @p node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param node - pointer to @a %s_list_node struct\n",
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
              " *  void %s_list_free_node_func(%s_list_node *node)\n",
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  llist helper function, frees node\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  node - pointer to %s_list_node struct\n",
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
   *  @fn void emit_aggregate_list_cmp_node_annotation(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *aggregate_name,
   *                                                   char *function_prefix,
   *                                                   int indent)
   *
   *  @brief emits annotation for aggregate list cmp node function
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
  
static void emit_aggregate_list_cmp_node_annotation(FILE *outfile,
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
              " *  @fn %s_list_node *%s_list_cmp_node_func(%s_list_node *a, "
              "%s_list_node *b)\n",
              aggregate_name,
              function_prefix,
              aggregate_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief llist helper function, compares @p a to @p b\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param a - pointer to @a %s_list_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param b - pointer to @a %s_list_node struct\n",
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

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  fn %s_list_node *%s_list_cmp_node_func(llist_node *a, "
              "llist_node *b)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  llist helper function, compares a to b\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  a - pointer to %s_list_node struct\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  b - pointer to %s_list_node struct\n",
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

