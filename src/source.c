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
 *  @file source.c
 *  @brief Generates source code from C declarations file (in XML)
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is C language source code
 */

#include <string.h>
#include <libgen.h>

#include "config.h"

#include "source.h"
#include "source-array.h"
#include "source-list.h"
#include "source-avl.h"
#include "options.h"

static void emit_enum_functions(FILE *outfile,
                                xmlNodePtr node,
                                char *project_name);
static void emit_aggregate_functions(FILE *outfile,
                                     xmlNodePtr node,
                                     char *project_name);
static void emit_aggregate_new_function(FILE *outfile,
                                        xmlNodePtr node,
                                        char *project,
                                        int indent);
static void emit_aggregate_dup_function(FILE *outfile,
                                        xmlNodePtr node,
                                        char *project,
                                        int indent);
static void emit_aggregate_free_function(FILE *outfile,
                                         xmlNodePtr node,
                                         char *project,
                                         int indent);
static void emit_aggregate_getters_and_setters(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent);
static void emit_aggregate_getter_function(FILE *outfile,
                                           xmlNodePtr node,
                                           char *project,
                                           char *aggregate_name,
                                           int indent);
static void emit_aggregate_setter_function(FILE *outfile,
                                           xmlNodePtr node,
                                           char *project,
                                           char *aggregate_name,
                                           int indent);

static void emit_source_annotation(FILE *outfile, char *file_name);
static void emit_enum_str_to_type_annotation(FILE *outfile,
                                             xmlNodePtr node,
                                             char *aggregate_name,
                                             char *function_prefix,
                                             int indent);
static void emit_enum_type_to_str_annotation(FILE *outfile,
                                             xmlNodePtr node,
                                             char *aggregate_name,
                                             char *function_prefix,
                                             int indent);
static void emit_aggregate_new_annotation(FILE *outfile,
                                          xmlNodePtr node,
                                          char *aggregate_name,
                                          char *function_prefix,
                                          int indent);
static void emit_aggregate_dup_annotation(FILE *outfile,
                                          xmlNodePtr node,
                                          char *aggregate_name,
                                          char *function_prefix,
                                          int indent);
static void emit_aggregate_free_annotation(FILE *outfile,
                                           xmlNodePtr node,
                                           char *aggregate_name,
                                           char *function_prefix,
                                           int indent);
static void emit_aggregate_getter_annotation(FILE *outfile,
                                             char *field_type,
                                             char *pointers,
                                             char *function_name,
                                             char *aggregate_name,
                                             char *field_name,
                                             int indent);
static void emit_aggregate_setter_annotation(FILE *outfile,
                                             char *field_type,
                                             char *pointers,
                                             char *function_name,
                                             char *aggregate_name,
                                             char *field_name,
                                             int indent);

  /**
   *  @fn void gen_source(xmlDocPtr doc, char *base_name)
   *
   *  @brief generates C source code from enum, struct and union declarations
   *
   *  @param doc - xmlDocPtr containing declaration metadata
   *  @param base_name - basic name of project for output files
   *
   *  @par Returns
   *  Nothing.
   */
  
void gen_source(xmlDocPtr doc, char *base_name)
{
  xmlNodePtr root;
  xmlNodePtr node;
  FILE *outfile = NULL;
  char *outfile_name = NULL;
  char *project_name = NULL;
  char *tmp = NULL;

  if (!doc || !base_name) return;

  node = xmlDocGetRootElement(doc);
  if (!node) goto exit;

  root = xmlDocGetRootElement(doc);
  if (!root) goto exit;

  if (!root->children) goto exit;

  if (strcmp((char *)node->name, "c-decls")) goto exit;

  outfile_name = malloc(strlen(base_name) + 3);
  if (!outfile_name) goto exit;

  sprintf(outfile_name, "%s.c", base_name);

  outfile = fopen(outfile_name, "w");
  if (!outfile) goto exit;

  project_name = get_project_name(base_name);
  if (!project_name) goto exit;

  str_upper(project_name);

  tmp = strdup(project_name);
  if (!tmp) goto exit;

  str_lower(tmp);

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

  fprintf(outfile, "#warning find and replace all occurences of USER ANNOTATION, then remove this line\n\n");

  emit_source_annotation(outfile, basename(outfile_name));

    // Emit front matter for source file

  fprintf(outfile, "#include <stdlib.h>\n");
  fprintf(outfile, "#include <stdio.h>\n");
  fprintf(outfile, "#include <string.h>\n");
  fprintf(outfile, "\n");

  fprintf(outfile, "#include \"%s.h\"\n", tmp);
  fprintf(outfile, "\n");

  free(tmp);
  tmp = NULL;

    // Emit functions for all enums, structs, and unions

  for (node = root->children; node; node = node->next)
  {
    if (!strcmp((char *)node->name, "enum"))
      emit_enum_functions(outfile, node, project_name);
    else if (!strcmp((char *)node->name, "struct") ||
             !strcmp((char *)node->name, "union"))
    {
      emit_aggregate_functions(outfile, node, project_name);
      emit_aggregate_array_functions(outfile, node, project_name);
      emit_aggregate_list_functions(outfile, node, project_name);
      emit_aggregate_avl_functions(outfile, node, project_name);
    }
  }

exit:
  if (outfile) fclose(outfile);
  if (outfile_name) free(outfile_name);
  if (project_name) free(project_name);
  if (tmp) free(tmp);
}

  /**
   *  @fn void emit_enum_functions(FILE *outfile,
   *                               xmlNodePtr node,
   *                               char *project_name)
   *
   *  @brief generates C source code from enum element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing enum element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum_functions(FILE *outfile,
                                xmlNodePtr node,
                                char *project_name)
{
  char *project = NULL;
  char *name = NULL;
  char *fpre = NULL;
  int indent = 0;
  xmlNodePtr child;
  char *item_name = NULL;
  int first = 1;

  if (!outfile || !node || !project_name) goto exit;
  if (strcmp((char *)node->name, "enum")) goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  fpre = function_prefix(project, name);
  
  emit_indent(outfile, indent + 2);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " *  Utility functions for enum %s\n", name);

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

    // string to type

  emit_enum_str_to_type_annotation(outfile, node, name, fpre, indent + 1);

  fprintf(outfile,
          "%s %s_str_to_type(char *enum_name)\n",
          name,
          fpre);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!enum_name) return 0;\n");

  fprintf(outfile, "\n");

  for (first = 1, child = node->children; child; child = child->next)
  {
    if (strcmp((char *)child->name, "item")) continue;
    item_name = get_attribute(child, "name");

    if (item_name)
    {
      emit_indent(outfile, indent);
      if (!first) fprintf(outfile, "else ");
      fprintf(outfile,
              "if (!strcmp(enum_name, \"%s\")) return %s;\n",
              item_name,
              item_name);
    }

    if (item_name) free(item_name);
    item_name = NULL;

    first = 0;
  }

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return 0;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

    // type to string

  emit_enum_type_to_str_annotation(outfile, node, name, fpre, indent + 1);

  fprintf(outfile, "char *%s_type_to_str(%s type)\n", fpre, name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "switch (type)\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "{\n");

  ++indent;

  for (first = 1, child = node->children; child; child = child->next)
  {
    if (strcmp((char *)child->name, "item")) continue;
    item_name = get_attribute(child, "name");

    if (item_name)
    {
      emit_indent(outfile, indent);
      fprintf(outfile,
              "case %s: return \"%s\";\n",
              item_name,
              item_name);
    }

    if (item_name) free(item_name);
    item_name = NULL;

    first = 0;
  }

  --indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return \"UNKNOWN\";\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (project) free(project);
  if (name) free(name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_functions(FILE *outfile,
   *                                    xmlNodePtr node,
   *                                    char *project_name)
   *
   *  @brief generates C source code from struct or union element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project_name - string containing project name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_functions(FILE *outfile,
                                     xmlNodePtr node,
                                     char *project_name)
{
  char *project = NULL;
  char *name = NULL;
  int indent = 0;

  if (!outfile || !node || !project_name) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  project = strdup(project_name);
  if (!project) goto exit;
  str_lower(project);

  name = get_attribute(node, "name");
  if (!name) goto exit;

  emit_indent(outfile, indent + 2);
  fprintf(outfile, "/*\n");

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " *  Utility functions for %s %s\n", node->name, name);

  emit_indent(outfile, indent + 2);
  fprintf(outfile, " */\n");

  fprintf(outfile, "\n");

  emit_aggregate_new_function(outfile, node, project, indent);
  emit_aggregate_dup_function(outfile, node, project, indent);
  emit_aggregate_free_function(outfile, node, project, indent);
  emit_aggregate_getters_and_setters(outfile, node, project, indent);

exit:
  if (project) free(project);
  if (name) free(name);
}

  /**
   *  @fn void emit_aggregate_new_function(FILE *outfile,
   *                                       xmlNodePtr node,
   *                                       char *project,
   *                                       int indent)
   *
   *  @brief generates C source code to create a new struct or union from
   *         element in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_new_function(FILE *outfile,
                                        xmlNodePtr node,
                                        char *project,
                                        int indent)
{
  char *name = NULL;
  char *fpre = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  fpre = function_prefix(project, name);

  emit_aggregate_new_annotation(outfile, node, name, fpre, indent + 1);

  fprintf(outfile,
          "%s *%s_new(void)\n",
          name,
          fpre);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *instance = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "instance = malloc(sizeof(%s));\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (instance) memset(instance, 0, sizeof(%s));\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return instance;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_dup_function(FILE *outfile,
   *                                       xmlNodePtr node,
   *                                       char *project,
   *                                       int indent)
   *
   *  @brief generates C source code to duplicate struct or union from
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
  
static void emit_aggregate_dup_function(FILE *outfile,
                                        xmlNodePtr node,
                                        char *project,
                                        int indent)
{
  char *name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;
  xmlNodePtr child;
  xmlNodePtr child2;
  xmlNodePtr scalar;
  xmlNodePtr reference;
  int n_pointers = 0;
  arrays *arrs = NULL;
  char *tmp_s = NULL;
  char *type_name = NULL;
  char *type = NULL;
  char *reference_name = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  fpre = function_prefix(project, name);

  emit_aggregate_dup_annotation(outfile, node, name, fpre, indent + 1);

  fprintf(outfile,
          "%s *%s_dup(%s *instance)\n",
          name,
          fpre,
          name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "%s *new_instance = NULL;\n", name);

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "new_instance = malloc(sizeof(%s));\n", name);

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!new_instance) goto exit;\n");

  fprintf(outfile, "\n");

    // scalars are copied with memcpy(), all "rogue" pointer values will be
    // corrected below

  emit_indent(outfile, indent);
  fprintf(outfile, "memcpy(new_instance, instance, sizeof(%s));\n", name);

  fprintf(outfile, "\n");

    // Any pointer to non-scalar field must call that field's _dup function

  for (child = node->children; child; child = child->next)
  {
    scalar = reference = NULL;
    n_pointers = 0;

    if (strcmp((char *)child->name, "field")) continue;

    name = get_attribute(child, "name");

      // collect array levels, pointers, and scalar information

    for (child2 = child->children; child2; child2 = child2->next)
    {
      if (!strcmp((char *)child2->name, "text")) continue;

      if (!strcmp((char *)child2->name, "array"))
      {
        if (!arrs) arrs = array_levels(child2);
        scalar = array_find_scalar(child2);
        if (!scalar) reference = array_find_reference(child2);
        n_pointers = array_pointer_count(child2);
      }
      else if (!strcmp((char *)child2->name, "pointer"))
      {
        n_pointers = pointer_count(child2);
        scalar = pointer_find_scalar(child2);
        if (!scalar) reference = pointer_find_reference(child2);
        if (!arrs) arrs = pointer_find_array(child2);
      }
      else if (!strcmp((char *)child2->name, "scalar"))
        scalar = child2;
      else if (!strcmp((char *)child2->name, "type-reference"))
        reference = child2;
    }

      // pointers to non-scalars call that types' _dup function, setting the
      //   pointer to the return of the _dup function
      // pointers to scalars duplicate the target scalar and create a new
      //   pointer
      // arrays can be copied in place with correct number of loop levels

    if (n_pointers == 1 && reference && !arrs)
    {
      tmp_s = get_attribute(reference, "name");
      if (aggregates_find(type_cache, tmp_s))
      {
        fpre2 = function_prefix(project, tmp_s);
        emit_indent(outfile, indent);
        fprintf(outfile,
                "new_instance->%s = %s_dup(instance->%s);\n",
                name,
                fpre2,
                name);
        fprintf(outfile, "\n");
        free(fpre2);
      }
      else
      {
        fprintf(outfile,
                "#warning Place code to copy '%s %s' here\n", tmp_s, name);
      }
      free(tmp_s);
    }
    else if (n_pointers == 1 && scalar)
    {
      type_name = get_attribute(scalar, "type-name");
      if (type_name && !strcmp(type_name, "char"))
      {
        emit_indent(outfile, indent);
        fprintf(outfile, "if (instance->%s)\n", name);
        emit_indent(outfile, indent + 1);
        fprintf(outfile,
                "new_instance->%s = strdup(instance->%s);\n",
                name,
                name);
        fprintf(outfile, "\n");
      }
      else fprintf(outfile, "#warning Place code to copy '%s' here\n", name);
    }
    else if (arrs && n_pointers)
      fprintf(outfile, "#warning Place code to copy array '%s' with pointer here\n", name);
    else if (n_pointers)
      fprintf(outfile, "#warning Place code to copy '%s' here\n", name);
    else if (arrs && reference)
    {
      reference_name = get_attribute(reference, "name");
      fprintf(outfile, "#warning Place code to copy array '%s' with reference type '%s' here\n", name, reference_name);
      free(reference_name);
    }
    else if (reference)
    {
      type_name = type = NULL;

      for (child2 = child->children; child2; child2 = child2->next)
      {
        if (!strcmp((char *)child2->name, "type-reference"))
        {
          type_name = get_attribute(child2, "name");
          type = get_attribute(child2, "type");
          break;
        }
      }

      if (type_name && type && !strcmp(type, "struct"))
      {
        fpre2 = function_prefix(project, type_name);

        emit_indent(outfile, indent);
        fprintf(outfile, "%s *tmp_%s_struct = NULL;\n", type_name, name);

        emit_indent(outfile, indent);
        fprintf(outfile, "tmp_%s_struct = %s_dup(&(instance->%s));\n",
                name,
                fpre2,
                name);

        emit_indent(outfile, indent);
        fprintf(outfile,
                "memcpy(&new_instance->%s, tmp_%s_struct, sizeof(%s));\n",
                name,
                name,
                type_name);

        emit_indent(outfile, indent);
        fprintf(outfile, "%s_free(tmp_%s_struct);\n",
                fpre2,
                name);

        fprintf(outfile, "\n");

        free(fpre2);
      }
      else
        fprintf(outfile, "#warning Place code to copy '%s' here\n", name);

      if (type_name) free(type_name);
      if (type) free(type);
      type_name = type = NULL;
    }

    if (arrs) arrays_free(arrs);
    arrs = NULL;

    if (name) free(name);
    name = NULL;
  }

  fprintf(outfile, "exit:\n");

  emit_indent(outfile, indent);
  fprintf(outfile, "return new_instance;\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_free_function(FILE *outfile,
   *                                        xmlNodePtr node,
   *                                        char *project,
   *                                        int indent)
   *
   *  @brief generates C source code to free struct or union from
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
  
static void emit_aggregate_free_function(FILE *outfile,
                                         xmlNodePtr node,
                                         char *project,
                                         int indent)
{
  char *name = NULL;
  char *fpre = NULL;
  char *fpre2 = NULL;
  xmlNodePtr child;
  xmlNodePtr child2;
  xmlNodePtr scalar;
  xmlNodePtr reference;
  int n_pointers = 0;
  arrays *arrs = NULL;
  char *tmp_s = NULL;
  char *type_name = NULL;
  char *reference_name = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  name = get_attribute(node, "name");
  if (!name) goto exit;

  fpre = function_prefix(project, name);

  emit_aggregate_free_annotation(outfile, node, name, fpre, indent + 1);

  fprintf(outfile,
          "void %s_free(%s *instance)\n",
          fpre,
          name);
  fprintf(outfile, "{\n");

  ++indent;

  emit_indent(outfile, indent);
  fprintf(outfile, "if (!instance) goto exit;\n");

  fprintf(outfile, "\n");

    // Any pointer to non-scalar field must call that field's _dup function

  for (child = node->children; child; child = child->next)
  {
    scalar = reference = NULL;
    n_pointers = 0;

    if (strcmp((char *)child->name, "field")) continue;

    name = get_attribute(child, "name");

      // collect array levels, pointers, and scalar information

    for (child2 = child->children; child2; child2 = child2->next)
    {
      if (!strcmp((char *)child2->name, "text")) continue;

      if (!strcmp((char *)child2->name, "array"))
      {
        if (!arrs) arrs = array_levels(child2);
        scalar = array_find_scalar(child2);
        if (!scalar) reference = array_find_reference(child2);
        n_pointers = array_pointer_count(child2);
      }
      else if (!strcmp((char *)child2->name, "pointer"))
      {
        n_pointers = pointer_count(child2);
        scalar = pointer_find_scalar(child2);
        if (!scalar) reference = pointer_find_reference(child2);
        if (!arrs) arrs = pointer_find_array(child2);
      }
      else if (!strcmp((char *)child2->name, "scalar"))
        scalar = child2;
      else if (!strcmp((char *)child2->name, "type-reference"))
        reference = child2;
    }

      // pointers to non-scalars call that types' _dup function, setting the
      //   pointer to the return of the _dup function
      // pointers to scalars duplicate the target scalar and create a new
      //   pointer
      // arrays can be copied in place with correct number of loop levels

    if (n_pointers == 1 && reference && !arrs)
    {
      tmp_s = get_attribute(reference, "name");
      if (aggregates_find(type_cache, tmp_s))
      {
        fpre2 = function_prefix(project, tmp_s);
        emit_indent(outfile, indent);
        fprintf(outfile, "if (instance->%s)\n", name);
        emit_indent(outfile, indent + 1);
        fprintf(outfile,
                "%s_free(instance->%s);\n",
                fpre,
                name);
        fprintf(outfile, "\n");
        free(fpre2);
      }
      else
      {
        fprintf(outfile,
                "#warning Place code to free '%s %s' here\n", tmp_s, name);
      }
      free(tmp_s);
    }
    else if (n_pointers == 1 && scalar)
    {
      type_name = get_attribute(scalar, "type-name");
      if (type_name && !strcmp(type_name, "char"))
      {
        emit_indent(outfile, indent);
        fprintf(outfile, "if (instance->%s)\n", name);
        emit_indent(outfile, indent + 1);
        fprintf(outfile, "free(instance->%s);\n", name);
        fprintf(outfile, "\n");
      }
      else fprintf(outfile, "#warning Place code to free '%s' here\n", name);
    }
    else if (arrs && n_pointers)
      fprintf(outfile, "#warning Place code to free array '%s' with pointer here\n", name);
    else if (n_pointers)
      fprintf(outfile, "#warning Place code to free '%s' here\n", name);
    else if (arrs && reference)
    {
      reference_name = get_attribute(reference, "name");
      fprintf(outfile, "#warning Place code to free array '%s' with reference type '%s' here\n", name, reference_name);
      free(reference_name);
    }
    else if (reference)
      fprintf(outfile, "#warning Place code to free '%s' here\n", name);

    if (arrs) arrays_free(arrs);
    arrs = NULL;

    if (name) free(name);
    name = NULL;
  }

  emit_indent(outfile, indent);
  fprintf(outfile, "free(instance);\n");

  fprintf(outfile, "\n");

  fprintf(outfile, "exit:\n");

  --indent;

  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");

exit:
  if (name) free(name);
  if (fpre) free(fpre);
}

  /**
   *  @fn void emit_aggregate_getters_and_setters(FILE *outfile,
   *                                              xmlNodePtr node,
   *                                              char *project,
   *                                              int indent)
   *
   *  @brief generates getter and setter C source code for struct or union
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
  
static void emit_aggregate_getters_and_setters(FILE *outfile,
                                               xmlNodePtr node,
                                               char *project,
                                               int indent)
{
  xmlNodePtr child;
  char *aggregate_name = NULL;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "struct") &&
      strcmp((char *)node->name, "union"))
    goto exit;

  aggregate_name = get_attribute(node, "name");
  if (!aggregate_name) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (strcmp((char *)child->name, "field")) continue;
    emit_aggregate_getter_function(outfile,
                                   child,
                                   project,
                                   aggregate_name,
                                   indent);
    emit_aggregate_setter_function(outfile,
                                   child,
                                   project,
                                   aggregate_name,
                                   indent);
  }

exit:
  if (aggregate_name) free(aggregate_name);
}

  /**
   *  @fn void emit_aggregate_getter_function(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *project,
   *                                          char *aggregate_name,
   *                                          int indent)
   *
   *  @brief generates getter C source code for struct or union from element
   *         in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param aggregate_name - string containing name of struct or union
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_getter_function(FILE *outfile,
                                           xmlNodePtr node,
                                           char *project,
                                           char *aggregate_name,
                                           int indent)
{
  char *field_name = NULL;
  char *field_type = NULL;
  char *fpre = NULL;
  char *function_name = NULL;
  xmlNodePtr child = NULL;
  arrays *arrs = NULL;
  int n_pointers = 0;
  xmlNodePtr scalar = NULL;
  xmlNodePtr reference = NULL;
  char pointers[33];
  int i;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "field")) goto exit;

  memset(pointers, 0, 33);

  field_name = get_attribute(node, "name");
  if (!field_name) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "array"))
    {
      arrs = array_levels(child);
      scalar = array_find_scalar(child);
      if (!scalar) reference = array_find_reference(child);
      n_pointers = array_pointer_count(child);
    }
    else if (!strcmp((char *)child->name, "pointer"))
    {
      n_pointers = pointer_count(child);
      scalar = pointer_find_scalar(child);
      if (!scalar) reference = pointer_find_reference(child);
    }
    else if (!strcmp((char *)child->name, "scalar"))
      scalar = child;
    else if (!strcmp((char *)child->name, "type-reference"))
      reference = child;
  }

  if (!scalar && !reference) goto exit;

  if (scalar) field_type = get_attribute(scalar, "type-name");
  else field_type = get_attribute(reference, "name");

  if (!field_type) goto exit;

  for (i = 0; i < n_pointers; i++)
    pointers[i] = '*';

  fpre = function_prefix(project, aggregate_name);

  function_name = strapp(function_name, fpre);
  function_name = strapp(function_name, "_get_");
  function_name = strapp(function_name, field_name);

  if (reference && !n_pointers)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which returns a type reference"
            " OR remove this warning\n",
            function_name);
  }
  else if (arrs)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which returns an array"
            " OR remove this warning\n",
            function_name);
  }
  else if (n_pointers > 1)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which returns multi-level pointer"
            " OR remove this warning\n",
            function_name);
  }
  else if (n_pointers && strcmp(field_type, "char"))
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which returns pointer to something not a char"
            " OR remove this warning\n",
            function_name);
  }
  else if (reference)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which returns a type reference"
            " OR remove this warning\n",
            function_name);
  }
  else
  {
    emit_aggregate_getter_annotation(outfile,
                                     field_type,
                                     pointers,
                                     function_name,
                                     aggregate_name,
                                     field_name,
                                     indent + 1);

    fprintf(outfile,
            "%s %s%s(%s *instance)\n",
            field_type,
            pointers,
            function_name,
            aggregate_name);

    emit_indent(outfile, indent);
    fprintf(outfile, "{\n");

    ++indent;

    if (n_pointers)
    {
      emit_indent(outfile, indent);
      fprintf(outfile, "return instance ? instance->%s : NULL;\n", field_name);
    }
    else if (scalar)
    {
      emit_indent(outfile, indent);
      fprintf(outfile,
              "return instance ? instance->%s : 0;\n",
              field_name);
    }

    --indent;

    emit_indent(outfile, indent);
    fprintf(outfile, "}\n");
  }

  fprintf(outfile, "\n");

exit:
  if (arrs) arrays_free(arrs);
  if (field_name) free(field_name);
  if (field_type) free(field_type);
  if (fpre) free(fpre);
  if (function_name) free(function_name);
}

  /**
   *  @fn void emit_aggregate_setter_function(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *project,
   *                                          char *aggregate_name,
   *                                          int indent)
   *
   *  @brief generates setter C source code for struct or union from element
   *         in @p node
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param project - string containing project name
   *  @param aggregate_name - string containing name of struct or union
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_setter_function(FILE *outfile,
                                           xmlNodePtr node,
                                           char *project,
                                           char *aggregate_name,
                                           int indent)
{
  char *field_name = NULL;
  char *field_type = NULL;
  char *fpre = NULL;
  char *function_name = NULL;
  xmlNodePtr child = NULL;
  arrays *arrs = NULL;
  int n_pointers = 0;
  xmlNodePtr scalar = NULL;
  xmlNodePtr reference = NULL;
  char pointers[33];
  int i;

  if (!outfile || !node || !project) goto exit;
  if (strcmp((char *)node->name, "field")) goto exit;

  memset(pointers, 0, 33);

  field_name = get_attribute(node, "name");
  if (!field_name) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "array"))
    {
      arrs = array_levels(child);
      scalar = array_find_scalar(child);
      if (!scalar) reference = array_find_reference(child);
      n_pointers = array_pointer_count(child);
    }
    else if (!strcmp((char *)child->name, "pointer"))
    {
      n_pointers = pointer_count(child);
      scalar = pointer_find_scalar(child);
      if (!scalar) reference = pointer_find_reference(child);
    }
    else if (!strcmp((char *)child->name, "scalar"))
      scalar = child;
    else if (!strcmp((char *)child->name, "type-reference"))
      reference = child;
  }

  if (!scalar && !reference) goto exit;

  if (scalar) field_type = get_attribute(scalar, "type-name");
  else field_type = get_attribute(reference, "name");

  if (!field_type) goto exit;

  for (i = 0; i < n_pointers; i++)
    pointers[i] = '*';

  fpre = function_prefix(project, aggregate_name);

  function_name = strapp(function_name, fpre);
  function_name = strapp(function_name, "_set_");
  function_name = strapp(function_name, field_name);

  if (reference && !n_pointers)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which sets the values for a type reference"
            " OR remove this warning\n",
            function_name);
  }
  else if (arrs)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which sets the values for an array"
            " OR remove this warning\n",
            function_name);
  }
  else if (n_pointers > 1)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which sets a multi-level pointer"
            " OR remove this warning\n",
            function_name);
  }
  else if (n_pointers && strcmp(field_type, "char"))
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which sets a pointer to something not a 'char'"
            " OR remove this warning\n",
            function_name);
  }
  else if (reference)
  {
    fprintf(outfile,
            "#warning Create function %s() here,"
            " which sets the values for a type reference"
            " OR remove this warning\n",
            function_name);
  }
  else
  {
    emit_aggregate_setter_annotation(outfile,
                                     field_type,
                                     pointers,
                                     function_name,
                                     aggregate_name,
                                     field_name,
                                     indent + 1);

    fprintf(outfile,
            "void %s(%s *instance, %s %s%s)\n",
            function_name,
            aggregate_name,
            field_type,
            pointers,
            field_name);

    emit_indent(outfile, indent);
    fprintf(outfile, "{\n");

    ++indent;

    emit_indent(outfile, indent);
    fprintf(outfile, "if (!instance) return;\n");

    fprintf(outfile, "\n");

    if (n_pointers)
    {
      emit_indent(outfile, indent);
      fprintf(outfile,
              "if (instance->%s) free(instance->%s);\n",
              field_name,
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              "instance->%s = NULL;\n",
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              "if (%s) instance->%s = strdup(%s);\n",
              field_name,
              field_name,
              field_name);
    }
    else if (scalar)
    {
      emit_indent(outfile, indent);
      fprintf(outfile,
              "if (instance) instance->%s = %s;\n",
              field_name,
              field_name);
    }

    --indent;

    emit_indent(outfile, indent);
    fprintf(outfile, "}\n");
  }

  fprintf(outfile, "\n");

exit:
  if (arrs) arrays_free(arrs);
  if (field_name) free(field_name);
  if (field_type) free(field_type);
  if (fpre) free(fpre);
  if (function_name) free(function_name);
}

  /**
   *  @fn void emit_source_annotation(FILE *outfile, char *file_name)
   *
   *  @brief emits global source annotation
   *
   *  @param outfile - open FILE * for writing
   *  @param file_name - string containing output file name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_source_annotation(FILE *outfile, char *file_name)
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
      break;
  }

exit:
}

  /**
   *  @fn void emit_enum_str_to_type_annotation(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *enum_name,
   *                                            char *function_prefix,
   *                                            int indent)
   *
   *  @brief emits annotation for enum function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param enum_name - string containing enum name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum_str_to_type_annotation(FILE *outfile,
                                             xmlNodePtr node,
                                             char *enum_name,
                                             char *function_prefix,
                                             int indent)
{
  if (!outfile || !node || !enum_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn %s %s_str_to_type(char *enum_name)\n",
              enum_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief converts @p enum_name to @a %s value\n",
              enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param enum_name - string version of @a %s value\n",
              enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @return @a %s value\n", enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s %s_str_to_type(char *enum_name)\n",
              enum_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  converts enum_name to %s value\n", enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    enum_name - string version of %s value\n",
              enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    %s value\n", enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_enum_type_to_str_annotation(FILE *outfile,
   *                                            xmlNodePtr node,
   *                                            char *enum_name,
   *                                            char *function_prefix,
   *                                            int indent)
   *
   *  @brief emits annotation for enum function
   *
   *  @param outfile - open FILE * for writing
   *  @param node - xmlNodePtr containing struct or union element
   *  @param enum_name - string containing enum name
   *  @param function_prefix - string containing leading part of function name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_enum_type_to_str_annotation(FILE *outfile,
                                             xmlNodePtr node,
                                             char *enum_name,
                                             char *function_prefix,
                                             int indent)
{
  if (!outfile || !node || !enum_name || !function_prefix) goto exit;
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
      fprintf(outfile,
              " *  @fn char *%s_type_to_str(%s type)\n",
              function_prefix,
              enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief converts @p type to string value\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param type - %s value\n",
              enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @return string version of %s value\n", enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  char *%s_type_to_str(%s type)\n",
              function_prefix,
              enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  converts type to string value\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    type - %s value\n", enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *    string version of %s value\n", enum_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_new_annotation(FILE *outfile,
   *                                         xmlNodePtr node,
   *                                         char *aggregate_name,
   *                                         char *function_prefix,
   *                                         int indent)
   *
   *  @brief emits annotation for aggregate new function
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
  
static void emit_aggregate_new_annotation(FILE *outfile,
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
              " *  @fn %s *%s_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief creates a new @a %s @a %s\n",
              node->name,
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
              " *  @return pointer to new @a %s @a %s on success, NULL on failure\n",
              node->name,
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
              " *  %s *%s_new(void)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  creates a new %s %s\n",
              node->name,
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
              " *    pointer to new %s %s on success, NULL on failure\n",
              node->name,
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
   *  @fn void emit_aggregate_dup_annotation(FILE *outfile,
   *                                         xmlNodePtr node,
   *                                         char *aggregate_name,
   *                                         char *function_prefix,
   *                                         int indent)
   *
   *  @brief emits annotation for aggregate dup function
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
  
static void emit_aggregate_dup_annotation(FILE *outfile,
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
              " *  @fn %s *%s_dup(%s *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief creates a deep copy of @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s @a %s\n",
              node->name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return pointer to new @a %s @a %s on success, NULL on failure\n",
              node->name,
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
              " *  %s *%s_dup(%s *instance)\n",
              aggregate_name,
              function_prefix,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  creates a deep copy of instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s %s\n",
              node->name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    pointer to new %s %s on success, NULL on failure\n",
              node->name,
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
   *  @fn void emit_aggregate_free_annotation(FILE *outfile,
   *                                          xmlNodePtr node,
   *                                          char *aggregate_name,
   *                                          char *function_prefix,
   *                                          int indent)
   *
   *  @brief emits annotation for aggregate free function
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
  
static void emit_aggregate_free_annotation(FILE *outfile,
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
              " *  @fn void %s_free(%s *instance)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *  @brief frees all memory allocated to @p instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s @a %s\n",
              node->name,
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
              " *  void %s_free(%s *instance)\n",
              aggregate_name,
              function_prefix);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  frees all memory allocated to instance\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s %s\n",
              node->name,
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
   *  @fn void emit_aggregate_getter_annotation(FILE *outfile,
   *                                            char *field_type,
   *                                            char *pointers,
   *                                            char *function_name,
   *                                            char *aggregate_name,
   *                                            char *field_name,
   *                                            int indent)
   *
   *  @brief emits annotation for aggregate field getter function
   *
   *  @param outfile - open FILE * for writing
   *  @param field_type - string containing type name of field
   *  @param pointers - string containing preformatted pointer indicators
   *  @param function_name - string containing function name
   *  @param aggregate_name - string containing aggregate name
   *  @param field_name - string containing field name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_getter_annotation(FILE *outfile,
                                             char *field_type,
                                             char *pointers,
                                             char *function_name,
                                             char *aggregate_name,
                                             char *field_name,
                                             int indent)
{
  if (!outfile || !field_type || !pointers ||
      !function_name || !aggregate_name ||
      !field_name)
    goto exit;

  if (!option_annotation()) goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen:
      emit_indent(outfile, indent);
      fprintf(outfile, "/**\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @fn %s %s%s(%s *instance)\n",
              field_type,
              pointers,
              function_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief returns value of instance->%s\n",
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @return %s%s on success, 0 or NULL on failure\n",
              field_type,
              pointers);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    case annotation_type_text:
      emit_indent(outfile, indent);
      fprintf(outfile, "/*\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s %s%s(%s *instance)\n",
              field_type,
              pointers,
              function_name,
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    returns value of instance->%s\n",
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Returns\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  %s%s on success, 0 or NULL on failure\n",
              field_type,
              pointers);

      emit_indent(outfile, indent);
      fprintf(outfile, " */\n");

      fprintf(outfile, "\n");
      break;

    default: break;
  }

exit:
}

  /**
   *  @fn void emit_aggregate_setter_annotation(FILE *outfile,
   *                                            char *field_type,
   *                                            char *pointers,
   *                                            char *function_name,
   *                                            char *aggregate_name,
   *                                            char *field_name,
   *                                            int indent)
   *
   *  @brief emits annotation for aggregate field setter function
   *
   *  @param outfile - open FILE * for writing
   *  @param field_type - string containing type name of field
   *  @param pointers - string containing preformatted pointer indicators
   *  @param function_name - string containing function name
   *  @param aggregate_name - string containing aggregate name
   *  @param field_name - string containing field name
   *  @param indent - indent level for output
   *
   *  @par Returns
   *  Nothing.
   */
  
static void emit_aggregate_setter_annotation(FILE *outfile,
                                             char *field_type,
                                             char *pointers,
                                             char *function_name,
                                             char *aggregate_name,
                                             char *field_name,
                                             int indent)
{
  if (!outfile || !field_type || !pointers ||
      !function_name || !aggregate_name ||
      !field_name)
    goto exit;

  if (!option_annotation()) goto exit;

  switch (option_annotation())
  {
    case annotation_type_doxygen:
      emit_indent(outfile, indent);
      fprintf(outfile, "/**\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @fn void %s(%s *instance, %s %s%s)\n",
              function_name,
              aggregate_name,
              field_type,
              pointers,
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @brief sets value of instance->%s\n",
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param instance - pointer to @a %s\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  @param %s - new value\n",
              field_name);

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
              " *  void %s(%s *instance, %s %s%s)\n",
              function_name,
              aggregate_name,
              field_type,
              pointers,
              field_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *  sets value of instance->%s\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile, " *\n");

      emit_indent(outfile, indent);
      fprintf(outfile, " *  Parameters\n");

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    instance - pointer to %s\n",
              aggregate_name);

      emit_indent(outfile, indent);
      fprintf(outfile,
              " *    %s - new value\n",
              field_name);

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

