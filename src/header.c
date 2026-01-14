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
 *  @file header.c
 *  @brief Converts C declarations to header
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
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

#include "header.h"
#include "header-array.h"
#include "header-list.h"
#include "header-avl.h"
#include "options.h"

  /*  Module specific function prototypes  */

static void emit_header_guard_start(FILE *outfile, char *project_name);
static void emit_header_guard_end(FILE *outfile, char *project_name);
static void emit_header_annotation(FILE *outfile, char *file_name);
static void emit_header_includes(FILE *outfile);
static void emit_typedef(FILE *outfile, xmlNodePtr node, int indent);
static void emit_typedef_annotation(FILE *outfile,
                                    xmlNodePtr node,
                                    char *name,
                                    int indent);
static void emit_enum(FILE *outfile, xmlNodePtr node, int indent);
static void emit_enum_annotation(FILE *outfile,
                                 xmlNodePtr node,
                                 char *name,
                                 int indent);
static void emit_enum_items(FILE *outfile, xmlNodePtr node, int indent);
static void emit_aggregate(FILE *outfile, xmlNodePtr node, int indent);
static void emit_aggregate_annotation(FILE *outfile,
                                      xmlNodePtr node,
                                      char *name,
                                      int indent);
static void emit_fields(FILE *outfile, xmlNodePtr node, int indent);
static void emit_type_reference(FILE *outfile, xmlNodePtr node, int indent);
static void emit_function_prototypes(FILE *outfile,
                                     xmlNodePtr node,
                                     char *project_name);
static void emit_enum_function_prototypes(FILE *outfile,
                                          xmlNodePtr node,
                                          char *project_name);
static void emit_aggregate_function_prototypes(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project_name);
static void emit_aggregate_field_function_prototypes(FILE *outfile,
                                                     xmlNodePtr node,
                                                     char *project_name,
                                                     char *aggregate_name,
                                                     char *sub_field_name);

  /**
   *  @fn void gen_header(xmlDocPtr doc, char *base_name)
   *
   *  @brief generates C header from enum, struct and union declarations
   *
   *  @param doc - xmlDocPtr containing declaration metadata
   *  @param base_name - basic name of project for output files
   *
   *  @par Returns
   *  Nothing.
   */
  
void gen_header(xmlDocPtr doc, char *base_name)
{
  xmlNodePtr root;
  xmlNodePtr node;
  FILE *outfile = NULL;
  char *outfile_name = NULL;
  char *project_name = NULL;

  if (!doc || !base_name) goto exit;

  root = xmlDocGetRootElement(doc);
  if (!root) goto exit;

  if (!root->children) goto exit;

  if (strcmp((char *)root->name, "c-decls")) goto exit;

  outfile_name = malloc(strlen(base_name) + 3);
  if (!outfile_name) goto exit;

  sprintf(outfile_name, "%s.h", base_name);

  outfile = fopen(outfile_name, "w");
  if (!outfile) goto exit;

  project_name = get_project_name(base_name);
  if (!project_name) goto exit;

  str_upper(project_name);

  switch (option_license())
  {
    case license_type_GPL_v3:
    case license_type_LGPL_v3:
      fprintf(outfile, "#warning change YEAR, FIRSTNAME and LASTNAME, then remove this line\n\n");
      break;

    default:
      break;
  }

  license_emit(outfile);

  emit_header_annotation(outfile, basename(outfile_name));

  emit_header_guard_start(outfile, project_name);

    // Emit warning about USER ANNOTATION

  fprintf(outfile, "#warning find and replace all occurences of USER ANNOTATION, then remove this line\n\n");

    // Emit project based include files

  emit_header_includes(outfile);

    // Emit typedefs for all enums, structs, and unions

  emit_indent(outfile, 2);
  fprintf(outfile, "/*\n");
  emit_indent(outfile, 2);
  fprintf(outfile, " *  Typedefs for all declared enums, structs and unions\n");
  emit_indent(outfile, 2);
  fprintf(outfile, " */\n");
  fprintf(outfile, "\n");

  for (node = root->children; node; node = node->next)
    emit_typedef(outfile, node, 0);

    // Emit all enums, structs, and unions

  emit_indent(outfile, 2);
  fprintf(outfile, "/*\n");
  emit_indent(outfile, 2);
  fprintf(outfile, " *  Declarations for all enums, structs and unions\n");
  emit_indent(outfile, 2);
  fprintf(outfile, " */\n");
  fprintf(outfile, "\n");

  for (node = root->children; node; node = node->next)
  {
    if (!strcmp((char *)node->name, "enum"))
    {
      emit_enum(outfile, node, 0);
      fprintf(outfile, ";\n\n");
    }
    else if (!strcmp((char *)node->name, "struct") ||
             !strcmp((char *)node->name, "union"))
    {
      emit_aggregate(outfile, node, 0);
      fprintf(outfile, ";\n\n");
      emit_aggregate_array(outfile, node, 0);
      fprintf(outfile, ";\n\n");
      emit_aggregate_list_node(outfile, node, 0);
      fprintf(outfile, ";\n\n");
      emit_aggregate_list(outfile, node, 0);
      fprintf(outfile, ";\n\n");
      emit_aggregate_avl_node(outfile, node, 0);
      fprintf(outfile, ";\n\n");
      emit_aggregate_avl(outfile, node, 0);
      fprintf(outfile, ";\n\n");
    }
    else
      continue;

  }

    // Emit function prototypes for above declarations

  emit_indent(outfile, 2);
  fprintf(outfile, "/*\n");
  emit_indent(outfile, 2);
  fprintf(outfile, " *  Function prototypes for all declared enums, structs"
                   " and unions\n");
  emit_indent(outfile, 2);
  fprintf(outfile, " */\n");
  fprintf(outfile, "\n");

  for (node = root->children; node; node = node->next)
    emit_function_prototypes(outfile, node, project_name);

  emit_header_guard_end(outfile, project_name);

exit:
  if (outfile) fclose(outfile);
  if (outfile_name) free(outfile_name);
  if (project_name) free(project_name);
}

  /**
   *  @fn void emit_enum(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits enum code from enum element in @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;

  if (!outfile || !node) goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  emit_enum_annotation(outfile, node, name, indent + 1);

  emit_indent(outfile, indent);
  fprintf(outfile, "typedef enum\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_enum_items(outfile, node->children, indent);

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "} %s", name);

exit:
  if (name) free(name);
}

  /**
   *  @fn void emit_enum_annotation(FILE *outfile,
   *                                xmlNodePtr node,
   *                                char *name,
   *                                int indent)
   *
   *  @brief emits annotation for an enum
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum
   *  @param name - string containing typedef name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum_annotation(FILE *outfile,
                                 xmlNodePtr node,
                                 char *name,
                                 int indent)
{
  if (!outfile || !node || !name) goto exit;
  if (!node->name) goto exit;

  if (!option_annotation()) goto exit;

  if (strcmp((char *)node->name, "enum"))
    goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen:
      emit_indent(outfile, indent);
      fprintf(outfile, "/**\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @typedef enum %s\n", name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a type for enum @a %s and values\n",
              name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  creates a type for enum %s and values\n", name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }


exit:
}

  /**
   *  @fn void emit_enum_items(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits enum items from @p node and siblings to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing item element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum_items(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *value = NULL;
  char *user_annotation = NULL;

  if (!outfile || !node) goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen:
      user_annotation = "/**< USER ANNOTATION */";
      break;

    case annotation_type_text:
    case annotation_type_none:
    default:
      break;
  }

  while (node)
  {
    name = value = NULL;

    if (!strcmp((char *)node->name, "item"))
    {
      name = get_attribute(node, "name");
      value = get_attribute(node, "value");

      if (name)
      {
        emit_indent(outfile, indent);

        fprintf(outfile,
                "%s = %s%s  %s\n",
                name,
                value ? value : "0",
                node->next && node->next->next ? "," : "",
                user_annotation ? user_annotation : "");
      }

      if (name) free(name);
      if (value) free(value);
    }

    node = node->next;
  }

exit:
}

  /**
   *  @fn void emit_aggregate(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits struct or union code from @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");

  if (name) emit_aggregate_annotation(outfile, node, name, indent + 1);

  emit_indent(outfile, indent);
  if (name)
    fprintf(outfile, "%s %s\n", (char *)node->name, name);
  else
    fprintf(outfile, "%s\n", (char *)node->name);

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  emit_fields(outfile, node->children, indent);

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}");

exit:
  if (name) free(name);
}

  /**
   *  @fn void emit_aggregate_annotation(FILE *outfile,
   *                                     xmlNodePtr node,
   *                                     char *name,
   *                                     int indent)
   *
   *  @brief emits annotation for a struct or union
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param name - string containing typedef name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_annotation(FILE *outfile,
                                      xmlNodePtr node,
                                      char *name,
                                      int indent)
{
  if (!outfile || !node || !name) goto exit;
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
      fprintf(outfile, " *  @%s %s\n", node->name, name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief declares %s @a %s\n", node->name, name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  declares %s %s\n", node->name, name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_fields(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits struct or union fields from @p node and siblings to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing field element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_fields(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  xmlNodePtr child = NULL;
  xmlNodePtr type_child = NULL;
  xmlNodePtr scalar = NULL;
  xmlNodePtr reference = NULL;
  unsigned int n_pointers;
  arrays *arrs = NULL;
  char *type_name = NULL;
  int i;

  if (!outfile || !node) goto exit;

  while (node)
  {
    name = NULL;
    n_pointers = 0;
    type_child = NULL;

    if (!strcmp((char *)node->name, "field"))
    {
        // Extract field name, can be anonymous for some types

      name = get_attribute(node, "name");

        // Iterate over children, getting type, pointers and array indices

      for (child = node->children; child; child = child->next)
      {
        if (!strcmp((char *)child->name, "text")) continue;

        if (!strcmp((char *)child->name, "array"))
        {
          arrs = array_levels(child);
          n_pointers = array_pointer_count(child);
          scalar = array_find_scalar(child);
          reference = array_find_reference(child);
        }
        else if (!strcmp((char *)child->name, "pointer"))
        {
          n_pointers = pointer_count(child);
          scalar = pointer_find_scalar(child);
          reference = pointer_find_reference(child);
        }
        else if (!strcmp((char *)child->name, "scalar"))
          type_name = get_attribute(child, "type-name");
        else if (!strcmp((char *)child->name, "enum") ||
                 !strcmp((char *)child->name, "struct") ||
                 !strcmp((char *)child->name, "union") ||
                 !strcmp((char *)child->name, "type-reference"))
          type_child = child;
      }

        // Output field components in proper order:
        //   Type name, pointers, field name, array indices

      if (scalar)
        type_name = get_attribute(scalar, "type-name");
      else if (reference)
        type_name = get_attribute(reference, "name");

      if (type_name)
      {
        emit_indent(outfile, indent);
        fprintf(outfile, "%s", type_name);
      }
      else if (type_child)
      {
        if (!strcmp((char *)type_child->name, "enum"))
          emit_enum(outfile, type_child, indent);
        else if (!strcmp((char *)type_child->name, "struct"))
          emit_aggregate(outfile, type_child, indent);
        else if (!strcmp((char *)type_child->name, "union"))
          emit_aggregate(outfile, type_child, indent);
        else if (!strcmp((char *)type_child->name, "type-reference"))
          emit_type_reference(outfile, type_child, indent);
      }

      if (name) fputc(' ', outfile);

      for (i = 0; i < n_pointers; i++)
        fputc('*', outfile);

      if (name) fprintf(outfile, "%s", name);

      if (arrs)
      {
        for (i = 0; i < arrs->n; i++)
        {
          if (arrs->array[i])
            fprintf(outfile, "[%d]", arrs->array[i]);
          else
            fprintf(outfile, "[]");
        }
      }

      if ((option_annotation() == annotation_type_doxygen) &&
          (!type_child || (type_child && name)))
        fprintf(outfile, ";  /**<  USER ANNOTATION */\n");
      else
        fprintf(outfile, ";\n");

      if (name) free(name);
      if (type_name) free(type_name);
      name = type_name = NULL;
      if (arrs) arrays_free(arrs);
      arrs = NULL;
      scalar = NULL;
      reference = NULL;
    }

    node = node->next;
  }

exit:
}

  /**
   *  @fn void emit_typedef(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits typedefs for enum, struct or union @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum, struct or union element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_typedef(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *array_name = NULL;
  char *list_name = NULL;
  char *avl_name = NULL;
  char *node_name = NULL;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "enum") &&
      strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  if (!strcmp((char *)node->name, "struct") ||
      !strcmp((char *)node->name, "union"))
  {
    emit_typedef_annotation(outfile, node, name, indent + 1);

    fprintf(outfile, "typedef %s %s %s;\n", node->name, name, name);

    fprintf(outfile, "\n");

    if (option_gen_array())
    {
      array_name = strapp(array_name, name);
      array_name = strapp(array_name, "_array");
      emit_typedef_annotation(outfile, node, array_name, indent + 1);
      fprintf(outfile, "typedef struct %s %s;\n", array_name, array_name);
      fprintf(outfile, "\n");
    }

    if (option_gen_list())
    {
      list_name = strapp(list_name, name);
      list_name = strapp(list_name, "_list");

      node_name = strapp(node_name, list_name);
      node_name = strapp(node_name, "_node");

      emit_typedef_annotation(outfile, node, node_name, indent + 1);
      fprintf(outfile, "typedef struct %s %s;\n", node_name, node_name);
      fprintf(outfile, "\n");

      emit_typedef_annotation(outfile, node, list_name, indent + 1);
      fprintf(outfile, "typedef struct %s %s;\n", list_name, list_name);
      fprintf(outfile, "\n");

      free(list_name);
      free(node_name);
      list_name = node_name = NULL;
    }

    if (option_gen_avl())
    {
      avl_name = strapp(avl_name, name);
      avl_name = strapp(avl_name, "_avl");

      node_name = strapp(node_name, avl_name);
      node_name = strapp(node_name, "_node");

      emit_typedef_annotation(outfile, node, node_name, indent + 1);
      fprintf(outfile, "typedef struct %s %s;\n", node_name, node_name);
      fprintf(outfile, "\n");

      emit_typedef_annotation(outfile, node, avl_name, indent + 1);
      fprintf(outfile, "typedef struct %s %s;\n", avl_name, avl_name);
      fprintf(outfile, "\n");

      emit_aggregate_avl_typedefs(outfile, node, indent);
      fprintf(outfile, ";\n");
      fprintf(outfile, "\n");

      free(avl_name);
      free(node_name);
      avl_name = node_name = NULL;
    }
  }

exit:
  if (name) free(name);
  if (array_name) free(array_name);
}

  /**
   *  @fn void emit_typedef_annotation(FILE *outfile,
   *                                   xmlNodePtr node,
   *                                   char *name,
   *                                   int indent)
   *
   *  @brief emits annotation for a typedef
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum, struct or union element
   *  @param name - string containing typedef name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_typedef_annotation(FILE *outfile,
                                    xmlNodePtr node,
                                    char *name,
                                    int indent)
{
  if (!outfile || !node || !name) goto exit;
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
      fprintf(outfile, " *  @typedef %s\n", name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a type for @a %s %s\n",
              name,
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
              " *  creates a type for %s %s\n",
              name,
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
   *  @fn void emit_type_reference(FILE *outfile, xmlNodePtr node, int indent)
   *
   *  @brief emits type-reference information in @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing type-reference element
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_type_reference(FILE *outfile, xmlNodePtr node, int indent)
{
  char *name = NULL;
  char *type = NULL;
  xmlAttrPtr attribute;

  if (!outfile || !node) goto exit;

  if (strcmp((char *)node->name, "type-reference"))
    goto exit;

  attribute = node->properties;
  while (attribute)
  {
    if (!strcmp((char *)attribute->name, "name"))
      name = (char *)xmlNodeGetContent(attribute->children);
    else if (!strcmp((char *)attribute->name, "type"))
      type = (char *)xmlNodeGetContent(attribute->children);

    if (name && type) break;

    attribute = attribute->next;
  } 

  if (!name || !type) goto exit;

  emit_indent(outfile, indent);

  fprintf(outfile, "%s %s", type, name);

exit:
  if (name) free(name);
  if (type) free(type);
}

  /**
   *  @fn void emit_header_guard_start(FILE *outfile, char *project_name)
   *
   *  @brief emits opening header guard macros to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_header_guard_start(FILE *outfile, char *project_name)
{
  if (!outfile || !project_name) return;

  fprintf(outfile, "#ifndef %s_H\n", project_name);
  fprintf(outfile, "#define %s_H\n", project_name);
  fprintf(outfile, "\n");
}

  /**
   *  @fn void emit_header_guard_end(FILE *outfile, char *project_name)
   *
   *  @brief emits closing header guard macros to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_header_guard_end(FILE *outfile, char *project_name)
{
  if (!outfile || !project_name) return;

  fprintf(outfile, "#endif //%s_H\n", project_name);
}

  /**
   *  @fn void emit_header_annotation(FILE *outfile, char *file_name)
   *
   *  @brief emits global header annotation
   *
   *  @param outfile - open FILE * for writing
   *  @param file_name - string containing output file name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_header_annotation(FILE *outfile, char *file_name)
{
  if (!outfile || !file_name) goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen:
      fprintf(outfile, "/**\n");
      fprintf(outfile, " *  @file %s\n", file_name);
      fprintf(outfile, " *  @brief USER ANNOTATION - brief\n");
      fprintf(outfile, " *\n");
      fprintf(outfile, " *  USER ANNOTATION - details\n");
      fprintf(outfile, " *\n");
      fprintf(outfile, " *  This file was generated by kahdifire\n");
      fprintf(outfile, " */\n");
      fprintf(outfile, "\n");
      break;
    case annotation_type_none:
    case annotation_type_text:
    default:
      fprintf(outfile, "/*\n");
      fprintf(outfile, " *\n");
      fprintf(outfile, " *  This file was generated by kahdifire\n");
      fprintf(outfile, " */\n");
      fprintf(outfile, "\n");
      break;
  }

exit:
}

  /**
   *  @fn void emit_header_includes(FILE *outfile)
   *
   *  @brief emits include directives for optional generators
   *
   *  @param outfile - open FILE * for writing
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_header_includes(FILE *outfile)
{
  int add_newline = 0;

  if (option_gen_list())
  {
    add_newline = 1;
    fprintf(outfile, "#include \"llist.h\"\n");
  }

  if (option_gen_avl())
  {
    add_newline = 1;
    fprintf(outfile, "#include \"avl.h\"\n");
  }

  if (add_newline) fprintf(outfile, "\n");
}

  /**
   *  @fn void emit_function_prototypes(FILE *outfile,
   *                                    xmlNodePtr node,
   *                                    char *project_name)
   *
   *  @brief emits utility function prototypes for enum, struct or union in
   *         @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum, struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_function_prototypes(FILE *outfile,
                                     xmlNodePtr node,
                                     char *project_name)
{
  if (!outfile || !node) return;

  if (!strcmp((char *)node->name, "enum"))
    emit_enum_function_prototypes(outfile, node, project_name);
  else if (!strcmp((char *)node->name, "struct") ||
           !strcmp((char *)node->name, "union"))
    emit_aggregate_function_prototypes(outfile, node, project_name);

  if (!strcmp((char *)node->name, "struct") ||
      !strcmp((char *)node->name, "union"))
  {
    emit_aggregate_array_function_prototypes(outfile, node, project_name);
    emit_aggregate_list_function_prototypes(outfile, node, project_name);
    emit_aggregate_avl_function_prototypes(outfile, node, project_name);
  }
}

  /**
   *  @fn void emit_enum_function_prototypes(FILE *outfile,
   *                                         xmlNodePtr node,
   *                                         char *project_name)
   *
   *  @brief emits utility function prototypes for enum in @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum_function_prototypes(FILE *outfile,
                                          xmlNodePtr node,
                                          char *project_name)
{
  char *project = NULL;
  char *name = NULL;

  if (!outfile || !node) goto exit;
  if (strcmp((char *)node->name, "enum")) goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  emit_indent(outfile, 1);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, 1);
  fprintf(outfile, " *  Utility functions for enum %s\n", name);

  emit_indent(outfile, 1);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "%s %s_%s_str_to_type(char *enum_name);\n", name, project, name);
  fprintf(outfile, "char *%s_%s_type_to_str(%s type);\n", project, name, name);

  fprintf(outfile, "\n");

exit:
  if (project) free(project);
  if (name) free(name);
}

  /**
   *  @fn void emit_aggregate_function_prototypes(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project_name)
   *
   *  @brief emits utility function prototypes for struct or union in @p node
   *         to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_function_prototypes(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project_name)
{
  xmlNodePtr child;
  char *project = NULL;
  char *name = NULL;
  char *function_prefix = NULL;

  if (!outfile || !node) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  if (strcmp(project, name))
  {
    function_prefix = strapp(function_prefix, project);
    function_prefix = strapp(function_prefix, "_");
    function_prefix = strapp(function_prefix, name);
  }
  else
    function_prefix = strdup(name);

  emit_indent(outfile, 1);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, 1);
  fprintf(outfile, " *  Utility functions for %s %s\n", node->name, name);

  emit_indent(outfile, 1);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

    // new, dup and free

  fprintf(outfile, "%s *%s_new(void);\n", name, function_prefix);
  fprintf(outfile, "%s *%s_dup(%s *instance);\n", name, function_prefix, name);
  fprintf(outfile, "void %s_free(%s *instance);\n", function_prefix, name);

  child = node->children;
  while (child)
  {
    if (!strcmp((char *)child->name, "field"))
    {
      emit_aggregate_field_function_prototypes(outfile,
                                               child,
                                               project,
                                               name,
                                               NULL);
    }

    child = child->next;
  }

  fprintf(outfile, "\n");

exit:
  if (project) free(project);
  if (name) free(name);
  if (function_prefix) free(function_prefix);
}

  /**
   *  @fn void emit_aggregate_field_function_prototypes(FILE *outfile,
   *                                                    xmlNodePtr node,
   *                                                    char *project_name,
   *                                                    char *aggregate_name,
   *                                                    char *sub_field_name)
   *
   *  @brief emits utility function prototypes for a struct or union field in
   *         @p node to @p outfile
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *  @param aggregate_name - string containing aggregate name
   *  @param sub_field_name - string containing sub_field name, if any
   *
   *  NOTE:  sub_field_name is for aggregates that contain embedded aggregates
   *         themselves.
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_field_function_prototypes(FILE *outfile,
                                                     xmlNodePtr node,
                                                     char *project_name,
                                                     char *aggregate_name,
                                                     char *sub_field_name)
{
  xmlNodePtr child;
  xmlNodePtr child2;
  xmlNodePtr scalar = NULL;
  xmlNodePtr reference = NULL;
  char *type_name = NULL;
  char *field_name = NULL;
  char *new_sub_field_name = NULL;
  char *tmp1, *tmp2;
  char *function_prefix = NULL;
  int i;
  int n_pointers = 0;
  int was_list = 0;

  if (!outfile || !node || !project_name || !aggregate_name) goto exit;

  if (strcmp((char *)node->name, "field")) goto exit;

  field_name = get_attribute(node, "name");

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "text")) continue;

    if (!strcmp((char *)child->name, "pointer"))
    {
      ++n_pointers;
      scalar = pointer_find_scalar(child);
      reference = pointer_find_reference(child);
      if (scalar)
        type_name = get_attribute(scalar, "type-name");
      else if (reference)
        type_name = get_attribute(reference, "name");
    }
    else if (!strcmp((char *)child->name, "array"))
    {
      ++n_pointers;
      scalar = array_find_scalar(child);
      reference = array_find_reference(child);
      if (scalar)
        type_name = get_attribute(scalar, "type-name");
      else if (reference)
        type_name = get_attribute(reference, "name");
    }
    else if (!strcmp((char *)child->name, "scalar"))
      type_name = get_attribute(child, "type-name");
    else if (!strcmp((char *)child->name, "type-reference"))
    {
      tmp1 = get_attribute(child, "type");
      tmp2 = get_attribute(child, "name");
      type_name = strapp(type_name, tmp1);
      type_name = strapp(type_name, " ");
      type_name = strapp(type_name, tmp2);
      if (tmp1) free(tmp1);
      if (tmp2) free(tmp2);
      ++n_pointers;
    }
    else if (!strcmp((char *)child->name, "struct") ||
             !strcmp((char *)child->name, "union"))
    {
      new_sub_field_name = strdup(aggregate_name);
      if (field_name)
      {
        new_sub_field_name = strapp(new_sub_field_name, "_");
        new_sub_field_name = strapp(new_sub_field_name, field_name);
      }

      for (child2 = child->children; child2; child2 = child2->next)
      {
        if (!strcmp((char *)child2->name, "field"))
        {
          emit_aggregate_field_function_prototypes(outfile,
                                                   child2,
                                                   project_name,
                                                   aggregate_name,
                                                   new_sub_field_name);
        }
      }

      free(new_sub_field_name);
      new_sub_field_name = NULL;

      was_list = 1;
    }

    scalar = reference = NULL;
  } 

  if (was_list) goto exit;

  if (!sub_field_name)
    new_sub_field_name = strdup(aggregate_name);
  else
    new_sub_field_name = strdup(sub_field_name);

  type_name = strapp(type_name, " ");

  for (i = 0; i < n_pointers; i++)
    type_name = strapp(type_name, "*");

  if (strcmp(project_name, new_sub_field_name))
  {
    function_prefix = strapp(function_prefix, project_name);
    function_prefix = strapp(function_prefix, "_");
    function_prefix = strapp(function_prefix, new_sub_field_name);
  }
  else
    function_prefix = strdup(new_sub_field_name);

  fprintf(outfile, "%s%s_get_%s(%s *instance);\n",
                   type_name,
                   function_prefix,
                   field_name,
                   aggregate_name);

  fprintf(outfile, "void %s_set_%s(%s *instance, %s%s);\n",
                   function_prefix,
                   field_name,
                   aggregate_name,
                   type_name,
                   field_name);

exit:
  if (type_name) free(type_name);
  if (field_name) free(field_name);
  if (function_prefix) free(function_prefix);
  if (new_sub_field_name) free(new_sub_field_name);
}

