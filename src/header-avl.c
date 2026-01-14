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
 *  @file header-avl.c
 *  @brief avl add-on to header.c
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>

#include "config.h"

#include "header-avl.h"
#include "options.h"

static void emit_aggregate_avl_typedefs_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *avl_name,
                                                   int indent);
static void emit_aggregate_avl_node_annotation(FILE *outfile,
                                                xmlNodePtr node,
                                                char *aggregate_name,
                                                char *node_name,
                                                int indent);
static void emit_aggregate_avl_annotation(FILE *outfile,
                                           xmlNodePtr node,
                                           char *aggregate_name,
                                           char *avl_name,
                                           int indent);

  /**
   *  @fn void emit_aggregate_avl_typedefs(FILE *outfile,
   *                                       xmlNodePtr node,
   *                                       int indent)
   *
   *  @brief emits typedef for avl action functions
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_avl_typedefs(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *avl_name = NULL;

  if (!option_gen_avl()) goto exit;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  avl_name = strapp(avl_name, name);
  avl_name = strapp(avl_name, "_avl");

  emit_aggregate_avl_typedefs_annotation(outfile,
                                         node,
                                         name,
                                         avl_name,
                                         indent + 1);

  fprintf(outfile,
          "typedef int (*%s_action)(%s_node *n)",
          avl_name,
          avl_name);

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
}

  /**
   *  @fn void emit_aggregate_avl(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits avl struct for struct or union from @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_avl(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *avl_name = NULL;
  int len;
  int is_doxygen = 0;

  if (!option_gen_avl()) goto exit;

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

  avl_name = strapp(avl_name, name);
  avl_name = strapp(avl_name, "_avl");

  emit_aggregate_avl_annotation(outfile, node, name, avl_name, indent + 1);

  emit_indent(outfile, indent);
  fprintf(outfile, "struct %s\n", avl_name ? avl_name : "!!ERROR!!");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  len = strlen(name) + 10;
  if (len < 16) len = 16;

  emit_indent(outfile, indent);
  fprintf(outfile,
          "%-*.*s/*%s  underlying avl  */\n",
          len,
          len,
          "avl *_avl;",
          is_doxygen ? "*<" : "");

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}");

exit:
  if (name) free(name);
  if (avl_name) free(avl_name);
}

  /**
   *  @fn void emit_aggregate_avl_node(FILE *outfile,
   *                                    xmlNodePtr node,
   *                                    int indent)
   *
   *  @brief emits avl node struct for struct or union from @p node to
   *         @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_avl_node(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *node_name = NULL;
  char *field = NULL;
  int len;
  int is_doxygen = 0;

  if (!option_gen_avl()) goto exit;

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
  node_name = strapp(node_name, "_avl_node");

  len = strlen(node_name) + 11;
  if (len < 22) len = 22;

  field = malloc(len);
  if (!field) goto exit;

  emit_aggregate_avl_node_annotation(outfile,
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
          node_name);

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}");

exit:
  if (name) free(name);
  if (node_name) free(node_name);
}

  /**
   *  @fn void emit_aggregate_avl_function_prototypes(FILE *outfile,
   *                                                   xmlNodePtr node,
   *                                                   char *project_name)
   *
   *  @brief emits utility avl function prototypes for struct or union in
   *         @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_aggregate_avl_function_prototypes(FILE *outfile,
                                             xmlNodePtr node,
                                             char *project_name)
{
  char *name = NULL;
  char *project = NULL;
  char *avl_name = NULL;
  char *function_prefix = NULL;

  if (!option_gen_avl()) goto exit;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  project = strdup(project_name);
  if (!project) goto exit;

  str_lower(project);

  avl_name = strdup(name);
  avl_name = strapp(avl_name, "_avl");

  if (strcmp(project, name))
  {
    function_prefix = strapp(function_prefix, project);
    function_prefix = strapp(function_prefix, "_");
    function_prefix = strapp(function_prefix, avl_name);
  }
  else
    function_prefix = strdup(avl_name);

  emit_indent(outfile, 1);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, 1);
  fprintf(outfile, " *  Utility functions for struct %s\n", avl_name);

  emit_indent(outfile, 1);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

    // new, dup, and free

  fprintf(outfile, "%s *%s_new(void);\n", avl_name, function_prefix);
  fprintf(outfile,
          "%s *%s_dup(%s *instance);\n",
          avl_name,
          function_prefix,
          avl_name);
  fprintf(outfile,
          "void %s_free(%s *instance);\n",
          function_prefix,
          avl_name);

    // insert and delete a node

  fprintf(outfile,
          "void %s_insert(%s *instance, %s *item);\n",
          function_prefix,
          avl_name,
          name);
  fprintf(outfile,
          "void %s_delete(%s *instance, %s *target);\n",
          function_prefix,
          avl_name,
          name);

    // avl user functions
    //   find and walk

  fprintf(outfile,
          "%s *%s_find(%s *instance, %s *needle);\n",
          name,
          function_prefix,
          avl_name,
          name);
  fprintf(outfile,
          "void %s_walk(%s *instance, avl_order order, %s_action action);\n",
          function_prefix,
          avl_name,
          avl_name);

    // avl helper functions
    //   new, dup, free, cmp

  fprintf(outfile,
          "%s_node *%s_new_node_func(void);\n",
          avl_name,
          function_prefix);
  fprintf(outfile,
          "%s_node *%s_dup_node_func(%s_node *node);\n",
          avl_name,
          function_prefix,
          avl_name);
  fprintf(outfile,
          "void %s_free_node_func(%s_node *node);\n",
          function_prefix,
          avl_name);
  fprintf(outfile,
          "int %s_cmp_node_func(%s_node *a, %s_node *b);\n",
          function_prefix,
          avl_name,
          avl_name);

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (project) free(project);
  if (avl_name) free(avl_name);
  if (function_prefix) free(function_prefix);
}

  /**
   *  @fn void emit_aggregate_avl_node_annotation(FILE *outfile,
   *                                               xmlNodePtr node,
   *                                               char *aggregate_name,
   *                                               char *avl_name,
   *                                               int indent)
   *
   *  @brief emits annotation for a node of an avl tree of structs or unions
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing typedef name of base aggregate
   *  @param avl_name - string containing typedef avl
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_avl_node_annotation(FILE *outfile,
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
              " *  @brief node struct used by avl tree of "
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
              " *  node struct used by avl tree of %s %ss\n",
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
   *  @fn void emit_aggregate_avl_annotation(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *aggregate_name,
   *                                          char *avl_name,
   *                                          int indent)
   *
   *  @brief emits annotation for an avl tree of structs or unions
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing typedef name of base aggregate
   *  @param avl_name - string containing typedef avl
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_avl_annotation(FILE *outfile,
                                           xmlNodePtr node,
                                           char *aggregate_name,
                                           char *avl_name,
                                           int indent)
{
  if (!outfile || !node || !aggregate_name || !avl_name) goto exit;
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
      fprintf(outfile, " *  @struct %s\n", avl_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief struct used to manage an avl tree of "
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
              " *  struct used to manage an avl tree of %s %ss\n",
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
   *  @fn static void emit_aggregate_avl_typedefs_annotation(FILE *outfile,
   *                                                        xmlNodePtr node,
   *                                                        char *aggregate_name,
   *                                                        char *avl_name,
   *                                                        int indent)
   *
   *  @brief emits annotation for avl tree typedefs
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param aggregate_name - string containing typedef name of base aggregate
   *  @param avl_name - string containing typedef avl
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_avl_typedefs_annotation(FILE *outfile,
                                                   xmlNodePtr node,
                                                   char *aggregate_name,
                                                   char *avl_name,
                                                   int indent)
{
  if (!outfile || !node || !aggregate_name || !avl_name) goto exit;
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
              " *  @typedef int (*%s_action)(%s_node *n);\n",
              avl_name,
              avl_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates type for function prototype used as action "
              "in calls to\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *         @a %s_walk()\n",
              avl_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  typedef int (*%s_action)(%s_node *n);\n",
              avl_name,
              avl_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates type for function prototype used as action "
              "in calls to\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *         %s_walk()\n",
              avl_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

