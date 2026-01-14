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
 *  @file header-list.c
 *  @brief list add-on to header.c
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>

#include "config.h"

#include "header-list.h"
#include "options.h"

static void emit_aggregate_list_node_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *node_name,
                                                int indent);
static void emit_aggregate_list_annotation(FILE *outfile,
                                           xmlNodePtr node,
                                           char *aggregate_name,
                                           char *list_name,
                                           int indent);

  /**
   *  @fn void emit_aggregate_list(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits list struct for struct or union from @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_list(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *list_name = NULL;
  int len;
  int is_doxygen = 0;

  if (!option_gen_list()) goto exit;

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

  list_name = strapp(list_name, name);
  list_name = strapp(list_name, "_list");

  emit_aggregate_list_annotation(outfile, node, name, list_name, indent + 1);

  emit_indent(outfile, indent);
  fprintf(outfile, "struct %s\n", list_name ? list_name : "!!ERROR!!");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  len = strlen(name) + 10;
  if (len < 16) len = 16;

  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s/*%s  underlying llist  */\n",
          len,
          len,
          "llist *_llist;",
          is_doxygen ? "*<" : "");

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}");

exit:
  if (name) free(name);
  if (list_name) free(list_name);
}

  /**
   *  @fn void emit_aggregate_list_node(FILE *outfile,
   *                                    xmlNodePtr node,
   *                                    int indent)
   *
   *  @brief emits list node struct for struct or union from @p node to
   *         @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_list_node(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *node_name = NULL;
  char *field = NULL;
  int len;
  int is_doxygen = 0;

  if (!option_gen_list()) goto exit;

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

  node_name = strapp(node_name, name);
  node_name = strapp(node_name, "_list_node");

  len = strlen(node_name) + 11;
  if (len < 22) len = 22;

  field = malloc(len);
  if (!field) goto exit;

  emit_aggregate_list_node_annotation(outfile,
                                      node,
                                      name,
                                      node_name,
                                      indent + 1);

  emit_indent(outfile, indent);
  fprintf(outfile, "struct %s\n", node_name);

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  sprintf(field, "%s *previous;", node_name);
  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s  /*%s  previous node  */\n",
          len,
          len,
          field,
          is_doxygen ? "*<" : "");

  sprintf(field, "%s *next;", node_name);
  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s  /*%s  next node      */\n",
          len,
          len,
          field,
          is_doxygen ? "*<" : "");

  sprintf(field, "%s data;", name);
  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s  /*%s  %s data  */\n",
          len,
          len,
          field,
          is_doxygen ? "*<" : "",
          name);

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}");

exit:
  if (name) free(name);
  if (node_name) free(node_name);
}

  /**
   *  @fn void emit_aggregate_list_function_prototypes(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *project_name)
   *
   *  @brief emits utility list function prototypes for struct or union in
   *         @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_list_function_prototypes(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project_name)
{
  char *name = NULL;
  char *project = NULL;
  char *list_name = NULL;
  char *function_prefix = NULL;

  if (!option_gen_list()) goto exit;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  project = strdup(project_name);
  if (!project) goto exit;

  str_lower(project);

  list_name = strdup(name);
  list_name = strapp(list_name, "_list");

  if (strcmp(project, name))
  {
    function_prefix = strapp(function_prefix, project);
    function_prefix = strapp(function_prefix, "_");
    function_prefix = strapp(function_prefix, list_name);
  }
  else
    function_prefix = strdup(list_name);

  emit_indent(outfile, 1);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, 1);
  fprintf(outfile, " *  Utility functions for struct %s\n", list_name);

  emit_indent(outfile, 1);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

    // new, dup, and free

  fprintf(outfile, "%s *%s_new(void);\n", list_name, function_prefix);
  fprintf(outfile,
          "%s *%s_dup(%s *instance);\n",
          list_name,
          function_prefix,
          list_name);
  fprintf(outfile,
          "void %s_free(%s *instance);\n",
          function_prefix,
          list_name);

    // add or remove item

  fprintf(outfile,
          "void %s_add(%s *instance,\n"
          "            llist_position position,\n"
          "            %s *where,\n"
          "            %s *item);\n",
          function_prefix,
          list_name,
          name,
          name);
  fprintf(outfile,
          "void %s_remove(%s *instance, %s *item);\n",
          function_prefix,
          list_name,
          name);

    // list iteration functions:
    //   head, tail, current, previous, next

  fprintf(outfile,
          "%s *%s_head(%s *instance);\n",
          name,
          function_prefix,
          list_name);
  fprintf(outfile,
          "%s *%s_tail(%s *instance);\n",
          name,
          function_prefix,
          list_name);
  fprintf(outfile,
          "%s *%s_current(%s *instance);\n",
          name,
          function_prefix,
          list_name);
  fprintf(outfile,
          "%s *%s_previous(%s *instance);\n",
          name,
          function_prefix,
          list_name);
  fprintf(outfile,
          "%s *%s_next(%s *instance);\n",
          name,
          function_prefix,
          list_name);
  fprintf(outfile,
          "%s *%s_find(%s *instance, %s *needle);\n",
          name,
          function_prefix,
          list_name,
          name);

    // list helder functions
    //   new, dup, free, cmp

  fprintf(outfile,
          "%s_list_node *%s_new_node_func(void);\n",
          name,
          function_prefix);
  fprintf(outfile,
          "%s_list_node *%s_dup_node_func(%s_list_node *node);\n",
          name,
          function_prefix,
          name);
  fprintf(outfile,
          "void %s_free_node_func(%s_list_node *node);\n",
          function_prefix,
          name);
  fprintf(outfile,
          "int %s_cmp_node_func(%s_list_node *a, %s_list_node *b);\n",
          function_prefix,
          name,
          name);

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (project) free(project);
  if (list_name) free(list_name);
  if (function_prefix) free(function_prefix);
}

  /**
   *  @fn void emit_aggregate_list_node_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *list_name,
   *                                               int indent)
   *
   *  @brief emits annotation for a node of a list of structs or unions
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing typedef name of base aggregate
   *  @param list_name - string containing typedef list
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_list_node_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *node_name,
                                                int indent)
{
  if (!outfile || !node || !aggregate_name || !node_name) goto exit;
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
      fprintf(outfile, " *  @struct %s\n", node_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief node struct used by doubly linked list of "
              "@a %s %ss\n",
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
              " *  node struct used by doubly linked list of %s %ss\n",
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

  /**
   *  @fn void emit_aggregate_list_annotation(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *aggregate_name,
   *                                          char *list_name,
   *                                          int indent)
   *
   *  @brief emits annotation for a list of structs or unions
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing typedef name of base aggregate
   *  @param list_name - string containing typedef list
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_list_annotation(FILE *outfile,
                                           xmlNodePtr node,
                                           char *aggregate_name,
                                           char *list_name,
                                           int indent)
{
  if (!outfile || !node || !aggregate_name || !list_name) goto exit;
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
      fprintf(outfile, " *  @struct %s\n", list_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief struct used to manage a doubly linked list of "
              "@a %s %ss\n",
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
              " *  struct used to manage a doubly linked list of %s %ss\n",
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

