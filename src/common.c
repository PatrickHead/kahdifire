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
 *  @file common.c
 *  @brief Routines common to all kahdifire modules
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>

#include "config.h"

#include "header.h"
#include "source.h"
#include "makefile.h"
#include "readme.h"
#include "doxygen.h"

  /*  Prototypes for functions in this module  */

static aggregates *build_type_cache(xmlDocPtr doc);

  /*  global variables available to other modules  */

aggregates *type_cache = NULL;  /**<  global, list of found aggregate names  */

  /*  static module variables  */

static int _indent = 2;         /**<  module, number of spaces per indent    */

  /**
   *  @fn int gen_code(char *file_name, char *base_name)
   *
   *  @brief generates C header and source code from enum, struct and union
   *         declarations
   *
   *  @param file_name - string containing file containing declarations
   *  @param base_name - basic name of project for output files
   *
   *  @return 0 on success
   *         -1 on failure
   */
  
int gen_code(char *file_name, char *base_name)
{
  FILE *infile = NULL;
  size_t len = 0L;
  size_t n_read = 0L;
  char *xml_buf = NULL;
  int retval = -1;
  xmlDocPtr doc = NULL;

  if (!file_name || !base_name) goto exit;

  len = get_file_size(file_name);
  if (!len) goto exit;

  infile = fopen(file_name, "r");
  if (!infile) goto exit;

  xml_buf = malloc(len + 1);
  if (!xml_buf) goto exit;

  n_read = fread(xml_buf, 1, len, infile);
  if (n_read != len) goto exit;

  doc = parse_xml(xml_buf);
  if (!doc) goto exit;

  type_cache = build_type_cache(doc);

  gen_header(doc, base_name);
  gen_source(doc, base_name);
  gen_makefile(doc, base_name);
  gen_readme(doc, base_name);
  gen_doxygen_configuration(doc, base_name);

  retval = 0;

exit:
  if (doc) xmlFreeDoc(doc);
  if (xml_buf) free(xml_buf);
  if (infile) fclose(infile);
  if (type_cache) aggregates_free(type_cache);

  return retval;
}

  /**
   *  @fn size_t get_file_size(char *file_name)
   *
   *  @brief returns size of @p file_name in bytes
   *
   *  @param file_name - string containing name of file
   *
   *  @return size of file in bytes
   *         0 if empty or on failure
   */
  
size_t get_file_size(char *file_name)
{
  struct stat st;

  if (!file_name) return 0;

  if (stat(file_name, &st)) return 0;

  return st.st_size;
}

  /**
   *  @fn xmlDocPtr parse_xml(char *buf)
   *
   *  @brief parses @p buf into XML document
   *
   *  @param buf - string containing well formed XML
   *
   *  @return xmlDocPtr on success
   *          NULL on failure
   */
  
xmlDocPtr parse_xml(char *buf)
{
  xmlDocPtr doc = NULL;
  int len = 0;

  if (!buf) goto exit;

  len = strlen(buf);

  doc = xmlParseMemory(buf, len);
  if (!doc) goto exit;

exit:
  return doc;
}

  /**
   *  @fn char *get_attribute(xmlNodePtr node, char *attr_name)
   *
   *  @brief gets value of @p attr_name from @p node
   *
   *  @param node - pointer to xmlNode
   *  @param attr_name - string containing name of attribute
   *
   *  @return string containing value of attribute on success
   *          NULL on failure
   */
  
char *get_attribute(xmlNodePtr node, char *attr_name)
{
  char *value = NULL;
  xmlAttr *attribute;

  if (!node) goto exit;

  attribute = node->properties;
  while (attribute)
  {
    if (!strcmp((char *)attribute->name, attr_name))
      value = (char *)xmlNodeListGetString(node->doc, attribute->children, 1);
    if (value) break;
    attribute = attribute->next;
  }

exit:
  return value;
}

  /**
   *  @fn char *create_base_name(char *file_name)
   *
   *  @brief creates a string from file_name with any extension removed 
   *
   *  @param file_name - string containing name of file
   *
   *  NOTE:  the caller is repsonsible for freeing the returned string
   *
   *  @return string with extenstion stripped on success
   *          NULL on failure
   */
  
char *create_base_name(char *file_name)
{
  char *base_name = NULL;
  char *dir_name = NULL;
  char *path = NULL;
  char *dot;
  char *s;

  if (!file_name) goto exit;

  s = strdup(file_name);
  if (!s) goto exit;
  dir_name = strdup(dirname(s));
  free(s);
  if (!dir_name) goto exit;

  s = strdup(file_name);
  if (!s) goto exit;
  base_name = strdup(basename(s));
  free(s);
  if (!base_name) goto exit;

  dot = strrchr(base_name, '.');
  if (dot) *dot = 0;

  path = malloc(strlen(dir_name) + strlen(base_name) + 2);
  if (!path) goto exit;
  sprintf(path, "%s/%s", dir_name, base_name);

exit:
  if (dir_name) free(dir_name);
  if (base_name) free(base_name);

  return path;
}

  /**
   *  @fn arrays *arrays_new(void)
   *
   *  @brief allocates and creates a new @a arrays struct
   *
   *  @par Parameters
   *  None.
   *
   *  @return pointer to new @a arrays struct on success
   *          NULL on failure
   */
  
arrays *arrays_new(void)
{
  arrays *as = NULL;

  as = malloc(sizeof(arrays));
  if (as) memset(as, 0, sizeof(arrays));

  return as;
}

  /**
   *  @fn void arrays_free(arrays *as)
   *
   *  @brief frees all memory allocated to @p as
   *
   *  @param as - pointer to @a arrays struct
   *
   *  @par Returns
   *  Nothing.
   */
  
void arrays_free(arrays *as)
{
  if (!as) return;
  if (as->array) free(as->array);
  free(as);
}

  /**
   *  @fn void arrays_add(arrays *as, int n_elements)
   *
   *  @brief adds a new array subscript to @p as
   *
   *  @param as - pointer to existing @a arrays struct
   *  @param n_elements - size of new subscript
   *
   *  @par Returns
   *  Nothing.
   */
  
void arrays_add(arrays *as, int n_elements)
{
  if (!as) return;

  as->array = realloc(as->array, sizeof(int) * (as->n + 1));
  as->array[as->n] = n_elements;

  ++as->n;
}

  /**
   *  @fn char *get_project_name(char *base_name)
   *
   *  @brief returns project name from @p base_name
   *
   *  @param base_name - basic name of project for output files
   *
   *  @return string containing project name on success
   *          NULL on failure
   */
  
char *get_project_name(char *base_name)
{
  char *project_name = NULL;
  char *temp = NULL;

  if (!base_name) goto exit;

  temp = strdup(base_name);

  project_name = strdup(basename(temp));

exit:
  if (temp) free(temp);

  return project_name;
}

  /**
   *  @fn void str_lower(char *str)
   *
   *  @brief converts all characters of @p str to lower case
   *
   *  @param str - string to convert
   *
   *  NOTE:  This function alters the existing contents of @p str
   *
   *  @par Returns
   *  Nothing.
   */
  
void str_lower(char *str)
{
  char *t;

  if (!str) return;

  t = str;
  while (*t)
  {
    *t = tolower(*t);
    ++t;
  }
}

  /**
   *  @fn void str_upper(char *str)
   *
   *  @brief converts all characters of @p str to upper case
   *
   *  @param str - string to convert
   *
   *  NOTE:  This function alters the existing contents of @p str
   *
   *  @par Returns
   *  Nothing.
   */
  
void str_upper(char *str)
{
  char *t;

  if (!str) return;

  t = str;
  while (*t)
  {
    *t = toupper(*t);
    ++t;
  }
}

  /**
   *  @fn void emit_indent(FILE *outfile, int indent)
   *
   *  @brief writes @p indent X @a _indent spaces to @p outfile
   *
   *  @param outfile - open FILE *
   *  @param indent - number of indents to emit
   *
   *  @par Returns
   *  Nothing.
   */
  
void emit_indent(FILE *outfile, int indent)
{
  int i;

  if (!outfile) return;

  for (i = 0; i < (indent * _indent) && (i < 256); ++i)
    fputc(' ', outfile);
}

  /**
   *  @fn aggregates *aggregates_new(void)
   *
   *  @brief allocates and creates a new @a aggregates struct
   *
   *  @par Parameters
   *  None.
   *
   *  @return pointer to new @a aggregates struct on success
   *          NULL on failure
   */
  
aggregates *aggregates_new(void)
{
  aggregates *as = NULL;

  as = malloc(sizeof(aggregates));
  if (as) memset(as, 0, sizeof(aggregates));

  return as;
}

  /**
   *  @fn void aggregates_free(aggregates *ags)
   *
   *  @brief frees all memory allocated to @p ags
   *
   *  @param ags - pointer to @a aggregates struct
   *
   *  @par Returns
   *  Nothing.
   */
  
void aggregates_free(aggregates *ags)
{
  int i;

  if (!ags) return;
  if (ags->array)
  {
    for (i = 0; i < ags->n; i++)
      free(ags->array[i]);
    free(ags->array);
  }
  free(ags);
}

  /**
   *  @fn void aggregates_add(aggregates *ags, char *name)
   *
   *  @brief adds a new aggregate name in @p name to @p ags
   *
   *  @param ags - pointer to existing @a aggregates struct
   *  @param name - string containing new aggregate name to add
   *
   *  @par Returns
   *  Nothing.
   */
  
void aggregates_add(aggregates *ags, char *name)
{
  if (!ags || !name) return;

  ags->array = realloc(ags->array, sizeof(int) * (ags->n + 1));
  ags->array[ags->n] = strdup(name);

  ++ags->n;
}

  /**
   *  @fn int aggregates_find(aggregates *ags, char *name)
   *
   *  @brief finds @p name in @p ags
   *
   *  @param ags - pointer to existing @a aggregates struct
   *  @param name - string containing aggregate name to find
   *
   *  @return 1 on success
   *          0 on failure
   */
  
int aggregates_find(aggregates *ags, char *name)
{
  int found = 0;
  int i;

  if (!ags || !name) goto exit;

  for (i = 0; i < ags->n; i++)
  {
    if (!strcmp(ags->array[i], name))
    {
      found = 1;
      break;
    }
  }

exit:
  return found;
}

  /**
   *  @fn static aggregates *build_type_cache(xmlDocPtr doc)
   *
   *  @brief creates a new @a aggregates list from @p doc
   *
   *  @param doc - xmlDocPtr which contains C declarations
   *
   *  @return pointer to @a aggregates struct on success
   *          NULL on failure
   */
  
static aggregates *build_type_cache(xmlDocPtr doc)
{
  aggregates *ags = NULL;
  xmlNodePtr root = NULL;
  xmlNodePtr node = NULL;
  char *name;

  if (!doc) goto exit;

  root = xmlDocGetRootElement(doc);
  if (!root) goto exit;

  if (strcmp((char *)root->name, "c-decls")) goto exit;

  ags = aggregates_new();
  if (!ags) goto exit;

  for (node = root->children; node; node = node->next)
  {
    if (!strcmp((char *)node->name, "struct") ||
        !strcmp((char *)node->name, "union"))
    {
      name = get_attribute(node, "name");
      if (!name) continue;

      aggregates_add(ags, name);
      free(name);
    }
  }

exit:
  return ags;
}

  /**
   *  @fn int pointer_count(xmlNodePtr node)
   *
   *  @brief counts the number of pointer levels in @p node
   *
   *  @param node - xmlNodePtr containing a pointer element
   *
   *  @return count of pointer levels on success
   *          0 on failure
   */
  
int pointer_count(xmlNodePtr node)
{
  int n = 0;
  xmlNodePtr child;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "pointer")) goto exit;

  ++n;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
      n += pointer_count(child);
  }

exit:
  return n;
}

  /**
   *  @fn xmlNodePtr pointer_find_scalar(xmlNodePtr node)
   *
   *  @brief locates scalar element in @p node
   *
   *  @param node - xmlNodePtr containing a pointer element
   *
   *  @return xmlNodePtr containing scalar element on success
   *          NULL on failure
   */
  
xmlNodePtr pointer_find_scalar(xmlNodePtr node)
{
  xmlNodePtr scalar = NULL;
  xmlNodePtr child = NULL;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "pointer")) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
    {
      scalar = pointer_find_scalar(child);
      break;
    }
    else if (!strcmp((char *)child->name, "scalar"))
    {
      scalar = child;
      break;
    }
  }

exit:
  return scalar;
}

  /**
   *  @fn xmlNodePtr pointer_find_reference(xmlNodePtr node)
   *
   *  @brief locates type-reference element in @p node
   *
   *  @param node - xmlNodePtr containing a pointer element
   *
   *  @return xmlNodePtr containing type-reference element on success
   *          NULL on failure
   */
  
xmlNodePtr pointer_find_reference(xmlNodePtr node)
{
  xmlNodePtr reference = NULL;
  xmlNodePtr child = NULL;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "pointer")) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
    {
      reference = pointer_find_reference(child);
      break;
    }
    else if (!strcmp((char *)child->name, "type-reference"))
    {
      reference = child;
      break;
    }
  }

exit:
  return reference;
}

  /**
   *  @fn arrays *pointer_find_array(xmlNodePtr node)
   *
   *  @brief returns @a arrays struct gleaned from @p node
   *
   *  @param node - xmlNodePtr containing a pointer element
   *
   *  @return pointer to @a arrays structure on success
   *          NULL on failure
   */
  
arrays *pointer_find_array(xmlNodePtr node)
{
  arrays *arr = NULL;
  xmlNodePtr child;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "pointer")) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
      arr = pointer_find_array(child);
    else if (!strcmp((char *)child->name, "array"))
    {
      arr = array_levels(child);
      break;
    }
  }

exit:
  return arr;
}

  /**
   *  @fn void array_level(arrays *arrs, xmlNodePtr node)
   *
   *  @brief collects array levels from @p node into @p arrs
   *
   *  @param arrs - string containing file containing declarations
   *  @param node - xmlNodePtr containing array and/or pointer elements
   *
   *  @par Returns
   *  Nothing.
   */
  
void array_level(arrays *arrs, xmlNodePtr node)
{
  char *tmp_s = NULL;
  xmlNodePtr child;

  if (!arrs || !node) goto exit;
  if (strcmp((char *)node->name, "array") &&
      strcmp((char *)node->name, "pointer"))
    goto exit;

  tmp_s = get_attribute(node, "n-elements");
  if (!tmp_s) goto exit;

  arrays_add(arrs, atoi(tmp_s));
  free(tmp_s);

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
    {
      array_level(arrs, child);
      break;
    }
    else if (!strcmp((char *)child->name, "array"))
    {
      array_level(arrs, child);
      break;
    }
  }

exit:
}

  /**
   *  @fn arrays *array_levels(xmlNodePtr node)
   *
   *  @brief collects array levels from @p node into @p arrs
   *
   *  @param node - xmlNodePtr containing array and/or pointer elements
   *
   *  NOTE:  This function calls @a array_level(), a recursive function
   *
   *  @return pointer to @a arrays struct on success
   *          NULL on failure
   */
  
arrays *array_levels(xmlNodePtr node)
{
  arrays *arrs = NULL;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "array")) goto exit;

  arrs = arrays_new();
  if (!arrs) goto exit;

  array_level(arrs, node);

exit:
  return arrs;
}

  /**
   *  @fn int array_level_count(xmlNodePtr node)
   *
   *  @brief returns count of array levels (subscripts) in @p node
   *
   *  @param node - xmlNodePtr containing array elements
   *
   *  @return count of array levels on success
   *          0 on failure
   */
  
int array_level_count(xmlNodePtr node)
{
  int levels = 0;
  xmlNodePtr child;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "array")) goto exit;

  ++levels;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "array"))
      levels += array_level_count(child);
    else if (!strcmp((char *)child->name, "pointer"))
      levels += pointer_count(child);
  }

exit:
  return levels;
}

  /**
   *  @fn int array_pointer_count(xmlNodePtr node)
   *
   *  @brief returns count of pointer levels (subscripts) in @p node
   *
   *  @param node - xmlNodePtr containing array elements
   *
   *  @return count of pointer levels on success
   *          0 on failure
   */
  
int array_pointer_count(xmlNodePtr node)
{
  int n_pointers = 0;
  xmlNodePtr child;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "array")) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
      n_pointers += pointer_count(child);
  }

exit:
  return n_pointers;
}

  /**
   *  @fn xmlNodePtr array_find_scalar(xmlNodePtr node)
   *
   *  @brief locates scalar element in @p node
   *
   *  @param node - xmlNodePtr containing array elements
   *
   *  @return xmlNodePtr containing scalar element on success
   *          NULL on failure
   */
  
xmlNodePtr array_find_scalar(xmlNodePtr node)
{
  xmlNodePtr scalar = NULL;
  xmlNodePtr child;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "array")) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
    {
      scalar = pointer_find_scalar(child);
      break;
    }
    else if (!strcmp((char *)child->name, "array"))
    {
      scalar = array_find_scalar(child);
      break;
    }
    else if (!strcmp((char *)child->name, "scalar"))
    {
      scalar = child;
      break;
    }
  }

exit:
  return scalar;
}

  /**
   *  @fn xmlNodePtr array_find_reference(xmlNodePtr node)
   *
   *  @brief locates type-reference element in @p node
   *
   *  @param node - xmlNodePtr containing array elements
   *
   *  @return xmlNodePtr containing type-reference element on success
   *          NULL on failure
   */
  
xmlNodePtr array_find_reference(xmlNodePtr node)
{
  xmlNodePtr reference = NULL;
  xmlNodePtr child;

  if (!node) goto exit;
  if (strcmp((char *)node->name, "array")) goto exit;

  for (child = node->children; child; child = child->next)
  {
    if (!strcmp((char *)child->name, "pointer"))
    {
      reference = pointer_find_reference(child);
      break;
    }
    else if (!strcmp((char *)child->name, "array"))
    {
      reference = array_find_reference(child);
      break;
    }
    else if (!strcmp((char *)child->name, "type-reference"))
    {
      reference = child;
      break;
    }
  }

exit:
  return reference;
}

  /**
   *  @fn char *function_prefix(char *project, char *declaration)
   *
   *  @brief creates a function prefix based on @p project and @p declaration
   *
   *  If @p project and @p declaration are the same, then the prefix will
   *  be the same as @p declaration.
   *
   *  If @p project and @p declaration differ, then the prefix will be
   *  @p project + "_" + @p declaration.
   *
   *  @param project - string containing name of project
   *  @param declaration - string containing name of declaration item
   *
   *  @return string containing proper function prefix
   */
  
char *function_prefix(char *project, char *declaration)
{
  char *prefix = NULL;

  if (!project || !declaration) goto exit;

  if (strcmp(project, declaration))
  {
    prefix = strapp(prefix, project);
    prefix = strapp(prefix, "_");
    prefix = strapp(prefix, declaration);
  }
  else
    prefix = strdup(declaration);

exit:
  return prefix;
}

