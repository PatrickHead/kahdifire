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
 *  @file c_decls_to_xml.cpp
 *  @brief GCC Plugin to parse C source code to struct/union/enum declarations.
 *
 *  Output is in XML format.
 *
 *  Inspired by source from https://github.com/Jongy/struct_layout
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcc-plugin.h>
#include <tree.h>
#include <print-tree.h>
#include <c-family/c-common.h>
#include <plugin-version.h>

#include "c_decls_to_xml.h"

int plugin_is_GPL_compatible; /**< must be defined for the plugin to run */

  // taken from linux/scripts/gcc-plugins/randomize_layout_plugin.c

#define ORIG_TYPE_NAME(node) \
  (TYPE_NAME(TYPE_MAIN_VARIANT(node)) != NULL_TREE ? ((const char *)IDENTIFIER_POINTER(TYPE_NAME(TYPE_MAIN_VARIANT(node)))) : NULL) /**<  type safe way to extract type name  */

  // from linux

#define container_of(ptr, type, member) ({              \
  const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
  (type *)( (char *)__mptr - offsetof(type,member) );}) /**< bullshit way to make C work like C++ */

  /*
   *  Function prototypes
   */

static bool is_basic_type(tree type);
static bool is_enum(const_tree type);
static bool is_struct(const_tree type);
static bool is_struct_or_union_or_enum(const_tree type);
static bool is_union(const_tree type);
static bool was_dumped(const char *name);
static const char *get_type_name(const_tree type);
static size_t get_field_size(const tree field_type);
static void add_to_dump_list(tree type);
static void add_to_dumped_structs(const char *name);
static void add_to_list(list *lst, list *item);
static void finish(void *event_data, void *user_data);
static void finish_type(void *event_data, void *user_data);
static void add_array(char **xml_str, tree field_type, int indent_level);
static void add_pointer(char **xml_str, tree field_type, int indent_level);
static void add_indent(char **xml_str, int n);
static void set_indent(int indent);
static void add_c_decl(char **xml_str,
                       const_tree base_type,
                       const char *name,
                       int indent_level);
static void add_enum_values(char **xml_str,
                            tree first_field,
                            size_t base_offset,
                            int indent_level);
static void add_fields(char **xml_str,
                       tree first_field,
                       size_t base_offset,
                       int indent_level);
static void add_field(char **xml_str,
                      tree field,
                      size_t base_offset,
                      int indent_level);
static void add_enum(char **xml_str,
                     const_tree base_type,
                     const char *name,
                     int indent_level);
static void add_struct(char **xml_str,
                      const_tree base_type,
                      const char *name,
                      int indent_level);
static void add_union(char **xml_str,
                      const_tree base_type,
                      const char *name,
                      int indent_level);
static void add_void(char **xml_str, int indent_level);
static void add_bitfield(char **xml_str, tree field, int indent_level);
static void add_function(char **xml_str, tree field, int indent_level);
static void add_scalar(char **xml_str, tree field, int indent_level);
static void add_user_type_reference(char **xml_str,
                                    const_tree base_type,
                                    const char *name,
                                    int indent_level);
static char *strapp(char *s1, char *s2);


  /*
   *  Globals
   */

static dumped_list dumped_structs;  /**<  list of decls already dumped       */
static dump_list to_dump;           /**<  list of decls to be dumped         */
static FILE *output_file;           /**<  name of output file, from options  */
static const char *target = NULL;   /**<  name of target struct              */
static int _indent = 2;             /**<  number of spaces per indent level  */
static char *_xml_str = NULL;       /**<  accumulative XML string buffer     */

  /*
   *  Function definitions
   */

  /**
   *  @fn bool was_dumped(const char *name)
   *
   *  @brief checks if @p name is in global @a dumped_structs list
   *
   *  @param name - string containing name of struct or union
   *
   *  @return @a true if @p name exists in @a dumped_structs
   *          @a false if @p name does not exist in @a dumped_structs
   */
  
static bool was_dumped(const char *name)
{
  list *iter = dumped_structs._list.next;

  while (iter)
  {
    dumped_list *n = container_of(iter, dumped_list, _list);

    if (!strcmp(name, n->name))
      return true;

    iter = iter->next;
  }

  return false;
}

  /**
   *  @fn void add_to_list(list *lst, list *item)
   *
   *  @brief adds list item @p n to list @p iter
   *
   *  @param lst - points to list node
   *  @param item - pointer to new list node
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_to_list(list *lst, list *item)
{
  while (lst->next != NULL)
    lst = lst->next;

  lst->next = item;
}

  /**
   *  @fn void add_to_dumped_structs(const char *name)
   *
   *  @brief adds struct @p name to global @a dumped_structs
   *
   *  @param name - string containing new struct name
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_to_dumped_structs(const char *name)
{
  const size_t len = strlen(name) + 1;
  dumped_list *n = (dumped_list*)xmalloc(sizeof(*n) + len);
  n->_list.next = NULL;
  memcpy(n->name, name, len);

  add_to_list(&dumped_structs._list, &n->_list);
}

  /**
   *  @fn void add_to_dump_list(tree type)
   *
   *  @brief adds @p type to global @a to_dump list
   *
   *  @param type - points to GCC type declaration node
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_to_dump_list(tree type)
{
  dump_list *n = (dump_list*)xmalloc(sizeof(*n));
  n->_list.next = NULL;
  n->type = type;

  add_to_list(&to_dump._list, &n->_list);
}

  // types that don't have another type beneath them.

  /**
   *  @fn bool is_basic_type(tree type)
   *
   *  @brief determines if @p type is a basic type.  basic types are those
   *         that don't have children types ie. are not aggregates
   *
   *  @param type - points to GCC type declaration node
   *
   *  @return @a true if @p type is a basic type
   *          @a false if @p type is not a basic type
   */
  
static bool is_basic_type(tree type)
{
  switch (TREE_CODE(type))
  {
    case INTEGER_TYPE:
    case BOOLEAN_TYPE:
    case ENUMERAL_TYPE:
    case REAL_TYPE:
    case RECORD_TYPE:
    case UNION_TYPE:
    case VOID_TYPE:
    case FUNCTION_TYPE:
      return true;

    default:
      return false;
  }
}

  /**
   *  @fn void add_array(char **xml_str, tree field_type, int indent_level)
   *
   *  @brief adds an array element and children to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param field_type - GCC node, points to field type of declaration
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_array(char **xml_str, tree field_type, int indent_level)
{
  size_t elem_size;
  size_t num_elem;
  size_t array_size;
  char num_str[32];
  tree content_type;
  const char *type_name = NULL;

  memset(num_str, 0, 32);

  elem_size = tree_to_uhwi(TYPE_SIZE_UNIT(TREE_TYPE(field_type)));
  array_size = get_field_size(field_type);

  if (!TYPE_SIZE_UNIT(field_type)) num_elem = 0;
  else if (elem_size == 0) num_elem = 0;
  else num_elem = tree_to_uhwi(TYPE_SIZE_UNIT(field_type)) / elem_size;

  if (!num_elem)
  {
    fprintf(stdout,
            "%s:%d Flexible Arrays are NOT Allowed.  Use pointers.\n",
            LOCATION_FILE(input_location),
            LOCATION_LINE(input_location));
    exit(EXIT_FAILURE);
  }

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<array size=\"");
  sprintf(num_str, "%lu", array_size);
  *xml_str = strapp(*xml_str, (char *)num_str);
  *xml_str = strapp(*xml_str, (char *)"\" n-elements=\"");
  sprintf(num_str, "%lu", num_elem);
  *xml_str = strapp(*xml_str, (char *)num_str);
  *xml_str = strapp(*xml_str, (char *)"\">\n");

  content_type = TREE_TYPE(field_type);
  if (content_type)
  {
    switch (TREE_CODE(content_type))
    {
      case POINTER_TYPE:
        add_pointer(xml_str, content_type, indent_level+1);
        break;

      case VECTOR_TYPE:
      case ARRAY_TYPE:
        add_array(xml_str, content_type, indent_level+1);
        break;

      case RECORD_TYPE:
      case UNION_TYPE:
      case ENUMERAL_TYPE:
        type_name = TYPE_IDENTIFIER(content_type) ?
                      get_type_name(content_type) : NULL;

        add_user_type_reference(xml_str, content_type, type_name, indent_level+1);
        break;

      default:
        add_scalar(xml_str, content_type, indent_level + 1);
        break;
    }
  }

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"</array>\n");
}

  /**
   *  @fn void add_pointer(char **xml_str, tree field_type, int indent_level)
   *
   *  @brief adds a user defined type reference to XML buffer
   *
   *  NOTE:  Will also add any associated pointer, array, user and scalar types
   *         that are associated with the pointer.
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param field_type - GCC node, points to field type of declaration
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_pointer(char **xml_str, tree field_type, int indent_level)
{
  size_t size;
  char size_str[32];
  tree content_type;
  const char *type_name;

  memset(size_str, 0, 32);

  size = get_field_size(field_type);

  sprintf(size_str, "%ld", size);

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<pointer size=\"");
  *xml_str = strapp(*xml_str, (char *)size_str);
  *xml_str = strapp(*xml_str, (char *)"\">\n");

  content_type = TREE_TYPE(field_type);
  if (content_type)
  {
    switch (TREE_CODE(content_type))
    {
      case POINTER_TYPE:
        add_pointer(xml_str, content_type, indent_level+1);
        break;

      case VECTOR_TYPE:
      case ARRAY_TYPE:
        add_array(xml_str, content_type, indent_level+1);
        break;

      case RECORD_TYPE:
      case UNION_TYPE:
      case ENUMERAL_TYPE:
        type_name = TYPE_IDENTIFIER(content_type) ?
                      get_type_name(content_type) : NULL;

        add_user_type_reference(xml_str, content_type, type_name, indent_level+1);
        break;

      default:
        add_scalar(xml_str, content_type, indent_level + 1);
        break;
    }
  }

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"</pointer>\n");
}

  /**
   *  @fn size_t get_field_size(const tree field_type)
   *
   *  @brief returns size of @p field_type in bits
   *
   *  @param field_type - points to GCC field type declaration
   *
   *  @return size of field type in bits on success
   *          0 if type has no known size
   */
  
static size_t get_field_size(const tree field_type)
{
  if (TYPE_SIZE(field_type))
    return tree_to_uhwi(TYPE_SIZE(field_type));

  return 0;
}

  /**
   *  @fn bool is_struct_or_union_or_enum(const_tree type)
   *
   *  @brief determines if @p type is a struct, union, or enum
   *
   *  @param type - points to GCC type declaration node
   *
   *  @return @a true if @p type is a struct, union or enum
   *          @a false if @p type is not a struct, union, or enum
   */
  
static bool is_struct_or_union_or_enum(const_tree type)
{
  return (is_struct(type) || is_union(type) || is_enum(type));
}

  /**
   *  @fn bool is_enum(const_tree type)
   *
   *  @brief determines if @p type is an enum
   *
   *  @param type - points to GCC type declaration node
   *
   *  @return @a true if @p type is an enum
   *          @a false if @p type is not an enum
   */
  
static bool is_enum(const_tree type)
{
  return (TREE_CODE(type) == ENUMERAL_TYPE);
}

  /**
   *  @fn bool is_struct(const_tree type)
   *
   *  @brief determines if @p type is a struct
   *
   *  @param type - points to GCC type declaration node
   *
   *  @return @a true if @p type is a struct
   *          @a false if @p type is not a struct
   */
  
static bool is_struct(const_tree type)
{
  return (TREE_CODE(type) == RECORD_TYPE);
}

  /**
   *  @fn bool is_union(const_tree type)
   *
   *  @brief determines if @p type is a union
   *
   *  @param type - points to GCC type declaration node
   *
   *  @return @a true if @p type is a union
   *          @a false if @p type is not a union
   */
  
static bool is_union(const_tree type)
{
  return (TREE_CODE(type) == UNION_TYPE);
}

  /**
   *  @fn const char *get_type_name(const_tree type)
   *
   *  @brief returns the underlying type name for structs/unions/enums and their
   *         typedefs.  returns the typedef name if the underlying type doesn't
   *         have a name.
   *
   *  @param type - points to GCC type declaration
   *
   *  @return string containing name of type on success
   *          NULL on failure
   */
  
static const char *get_type_name(const_tree type)
{
  const char *name = NULL;
  const char *type_id = NULL;
  const char *orig_name = NULL;

  if (!is_struct_or_union_or_enum(type)) goto exit;
  if (!TYPE_IDENTIFIER(type)) goto exit;

  type_id = IDENTIFIER_POINTER(TYPE_IDENTIFIER(type));
  if (!type_id) goto exit;

  orig_name = ORIG_TYPE_NAME(type);

  name = orig_name ? orig_name : type_id;

exit:

  return name;
}

  /**
   *  @fn void add_indent(char **xml_str, int indent)
   *
   *  @brief adds indent spaces to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param indent - number of indents to emit for each appended line
   *
   *  NOTE:  @a indent will be multiplied by global @a _indent
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_indent(char **xml_str, int indent)
{
  int i;
  char spaces[256];

  memset(spaces, 0, 256);

  for (i = 0; i < indent * _indent && i < 256; ++i)
    spaces[i] = ' ';

  *xml_str = strapp(*xml_str, spaces);
}

  /**
   *  @fn void add_enum_values(char **xml_str,
   *                           tree first_field,
   *                           size_t base_offset,
   *                           int indent_level)
   *
   *  @brief adds all item elements for an enum to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param first_field - GCC node, points to a first item declaration
   *  @param base_offset - offset of field in parent struct or union
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_enum_values(char **xml_str,
                            tree first_field,
                            size_t base_offset,
                            int indent_level)
{
  const char *field_name = NULL;
  const_tree decl_name = NULL;
  int value;
  char value_str[32];

  memset(value_str, 0, 32);

  for (tree field = first_field; field; field = TREE_CHAIN(field))
  {
    decl_name = TREE_PURPOSE(field);
    if (!decl_name) continue;

    field_name = IDENTIFIER_POINTER(decl_name);
    if (!field_name) continue;

    add_indent(xml_str, indent_level);

    value = TREE_INT_CST_LOW(TREE_VALUE(field));

    sprintf(value_str, "%d", value);

    *xml_str = strapp(*xml_str, (char *)"<item name=\"");
    *xml_str = strapp(*xml_str, (char *)field_name);
    *xml_str = strapp(*xml_str, (char *)"\" value=\"");
    *xml_str = strapp(*xml_str, (char *)value_str);
    *xml_str = strapp(*xml_str, (char *)"\"/>\n");
  }
}

  /**
   *  @fn void add_fields(char **xml_str,
   *                      tree first_field,
   *                      size_t base_offset,
   *                      int indent_level)
   *
   *  @brief adds a fields element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param first_field - GCC node, points to a first field declaration
   *  @param base_offset - offset of field in parent struct or union
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_fields(char **xml_str,
                       tree first_field,
                       size_t base_offset,
                       int indent_level)
{
  tree field;

  for (field = first_field; field; field = TREE_CHAIN(field))
  {
    if (TREE_CODE(field) != FIELD_DECL) continue;
    add_field(xml_str, field, base_offset, indent_level);
  }
}

  /**
   *  @fn void add_field(char **xml_str,
   *                     tree field,
   *                     size_t base_offset,
   *                     int indent_level)
   *
   *  @brief adds a field element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param field - GCC node, points to field declaration
   *  @param base_offset - offset of field in parent struct or union
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_field(char **xml_str,
                      tree field,
                      size_t base_offset,
                      int indent_level)
{
  size_t offset;
  tree t_offset;
  tree t_bit_offset;
  const char *field_name;
  const_tree decl_name;
  char offset_str[32];
  tree field_type;

  memset(offset_str, 0, 32);

  field_type = TREE_TYPE(field);

    // field offset
  offset = base_offset;

  if (field_type)
  {
    t_offset = DECL_FIELD_OFFSET(field);

    if (t_offset)
    {
      if (!((TREE_CODE(t_offset) == INTEGER_CST) && TREE_CONSTANT(t_offset)))
        return;

      offset += tree_to_uhwi(t_offset) * 8;
    }

      // add bit offset. there's an explanation about why it's required, see
      // macro declaration in tree.h

    t_bit_offset = DECL_FIELD_BIT_OFFSET(field);

    if (t_bit_offset)
    {
      if (!((TREE_CODE(t_bit_offset) == INTEGER_CST) &&
            TREE_CONSTANT(t_bit_offset)))
        return;

      offset += tree_to_uhwi(t_bit_offset);
    }
  }

  sprintf(offset_str, "%ld", offset);

    // field name

  field_name = NULL;

  decl_name = DECL_NAME(field);
  if (decl_name)
    field_name = IDENTIFIER_POINTER(decl_name);
  else
  {
      // unnamed bitfield, ignore and return
      // I've also seen integers used for struct padding, so I'll allow that
      // as well.  (e.g linux/include/uapi/linux/timex.h timex)

    if (DECL_BIT_FIELD(field) || TREE_CODE(field_type) == INTEGER_TYPE)
      return;

      // shouldn't be NULL, only allowed for anonymous unions.

    if (!is_struct_or_union_or_enum(field_type)) return;

    add_indent(xml_str, indent_level);
    *xml_str = strapp(*xml_str, (char *)"<field offset=\"");
    *xml_str = strapp(*xml_str, (char *)offset_str);
    *xml_str = strapp(*xml_str, (char *)"\">\n");

    add_c_decl(xml_str, field_type, NULL, indent_level);

    add_indent(xml_str, indent_level);
    *xml_str = strapp(*xml_str, (char *)"</field>\n");

    return;
  }

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<field");

  if (field_name)
  {
    *xml_str = strapp(*xml_str, (char *)" name=\"");
    *xml_str = strapp(*xml_str, (char *)field_name);
    *xml_str = strapp(*xml_str, (char *)"\"");
  }

  *xml_str = strapp(*xml_str, (char *)" offset=\"");
  *xml_str = strapp(*xml_str, (char *)offset_str);
  *xml_str = strapp(*xml_str, (char *)"\">\n");

  ++indent_level;

    // Add any array and pointer (reference types) here

  if (field_type && !is_basic_type(field_type))
  {
    switch (TREE_CODE(field_type))
    {
      case VECTOR_TYPE:
      case ARRAY_TYPE:
        add_array(xml_str, field_type, indent_level);
        break;

      case POINTER_TYPE:
      case REFERENCE_TYPE:
        add_pointer(xml_str, field_type, indent_level);
        break;

      default: break;
    }
  }
  else
  {
      // Add basic type here

    if (!TYPE_IDENTIFIER(field_type) &&          // anonymous struct/union
        is_struct_or_union_or_enum(field_type))
      add_c_decl(xml_str, field_type, NULL, indent_level - 1);
    else
    {
      if (is_struct_or_union_or_enum(field_type))  // ref to struct/union/enum
      {
        add_to_dump_list(field_type);
        add_user_type_reference(xml_str,
                                field_type,
                                TYPE_IDENTIFIER(field_type) ?
                                  get_type_name(field_type) :
                                  NULL,
                                indent_level);
      }
      else if (TREE_CODE(field_type) == VOID_TYPE)
        add_void(xml_str, indent_level);
      else if (DECL_BIT_FIELD(field))  // bitfields have no TYPE_IDENTIFIER
        add_bitfield(xml_str, field, indent_level);
      else if (TREE_CODE(field_type) == FUNCTION_TYPE)  // function pointer
        add_function(xml_str, field, indent_level);
      else
        add_scalar(xml_str, field_type, indent_level);
    }
  }

  --indent_level;

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"</field>\n");
}

  /**
   *  @fn void add_enum(char **xml_str,
   *                    const_tree base_type,
   *                    const char *name,
   *                    int indent_level)
   *
   *  @brief adds an enum element to XML buffer 
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param base_type - GCC node, points to base type of declaration
   *  @param name - name of node variable
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_enum(char **xml_str,
                     const_tree base_type,
                     const char *name,
                     int indent_level)
{
  char size_str[32];

  memset(size_str, 0, 32);

  sprintf(size_str, "%ld", tree_to_uhwi(TYPE_SIZE(base_type)));

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<enum");
  if (name)
  {
    *xml_str = strapp(*xml_str, (char *)" name=\"");
    *xml_str = strapp(*xml_str, (char *)name);
    *xml_str = strapp(*xml_str, (char *)"\"");
  }
  *xml_str = strapp(*xml_str, (char *)" size=\"");
  *xml_str = strapp(*xml_str, (char *)size_str);
  *xml_str = strapp(*xml_str, (char *)"\">\n");

  ++indent_level;

  add_enum_values(xml_str, TYPE_VALUES(base_type), 0, indent_level);

  --indent_level;

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"</enum>\n");
}

  /**
   *  @fn void add_struct(char **xml_str,
   *                      const_tree base_type,
   *                      const char *name,
   *                      int indent_level)
   *
   *  @brief adds a struct element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param base_type - GCC node, points to base type of declaration
   *  @param name - name of node variable
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_struct(char **xml_str,
                      const_tree base_type,
                      const char *name,
                      int indent_level)
{
  char size_str[32];

  memset(size_str, 0, 32);

  sprintf(size_str, "%ld", tree_to_uhwi(TYPE_SIZE(base_type)));

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<struct");
  if (name)
  {
    *xml_str = strapp(*xml_str, (char *)" name=\"");
    *xml_str = strapp(*xml_str, (char *)name);
    *xml_str = strapp(*xml_str, (char *)"\"");
  }
  *xml_str = strapp(*xml_str, (char *)" size=\"");
  *xml_str = strapp(*xml_str, (char *)size_str);
  *xml_str = strapp(*xml_str, (char *)"\">\n");

  ++indent_level;

  add_fields(xml_str, TYPE_FIELDS(base_type), 0, indent_level);

  --indent_level;

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"</struct>\n");
}

  /**
   *  @fn void add_union(char **xml_str,
   *                     const_tree base_type,
   *                     const char *name,
   *                     int indent_level)
   *
   *  @brief adds a union element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param base_type - GCC node, points to base type of declaration
   *  @param name - name of node variable
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_union(char **xml_str,
                      const_tree base_type,
                      const char *name,
                      int indent_level)
{
  char size_str[32];

  memset(size_str, 0, 32);

  sprintf(size_str, "%ld", tree_to_uhwi(TYPE_SIZE(base_type)));

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<union");
  if (name)
  {
    *xml_str = strapp(*xml_str, (char *)" name=\"");
    *xml_str = strapp(*xml_str, (char *)name);
    *xml_str = strapp(*xml_str, (char *)"\"");
  }
  *xml_str = strapp(*xml_str, (char *)" size=\"");
  *xml_str = strapp(*xml_str, (char *)size_str);
  *xml_str = strapp(*xml_str, (char *)"\">\n");

  ++indent_level;

  add_fields(xml_str, TYPE_FIELDS(base_type), 0, indent_level);

  --indent_level;

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"</union>\n");
}

  /**
   *  @fn static void add_void(char **xml_str, int indent_level)
   *
   *  @brief adds a void element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_void(char **xml_str, int indent_level)
{
  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<void/>\n");
}

  /**
   *  @fn void add_bitfield(char **xml_str, tree field, int indent_level)
   *
   *  @brief adds a bitfield element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param field - GCC node, points to field declaration
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_bitfield(char **xml_str, tree field, int indent_level)
{
#warning, add_bitfield() is BROKEN
  tree field_type;
  char num_str[32];

  field_type = TREE_TYPE(field);
  if (!field_type) return;

  memset(num_str, 0, 32);

  sprintf(num_str, "%ld", tree_to_uhwi(DECL_SIZE(field)));

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<bitfield size=\"");
  *xml_str = strapp(*xml_str, num_str);
  *xml_str = strapp(*xml_str, (char *)"\" signed=\"");
  *xml_str = strapp(*xml_str,
                    (char *)(TYPE_UNSIGNED(field_type) ? "true" : "false"));
  *xml_str = strapp(*xml_str, (char *)"\"/>\n");
}

  /**
   *  @fn void add_function(char **xml_str, tree field, int indent_level)
   *
   *  @brief adds a function element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param field - GCC node, points to field declaration
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_function(char **xml_str, tree field, int indent_level)
{
// this is not complete, may not ever be needed
  *xml_str = strapp(*xml_str, (char *)"<function/>\n");
}

  /**
   *  @fn void add_scalar(char **xml_str, tree field, int indent_level)
   *
   *  @brief adds a scalar element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param field - GCC node, points to field declaration
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_scalar(char **xml_str, tree field, int indent_level)
{
  const char *type_name_s = NULL;
  size_t field_size;
  char num_str[32];
  tree type_id;

  if (!field) return;

  memset(num_str, 0, 32);

  field_size = get_field_size(field);

  if (TREE_CODE(field) == ENUMERAL_TYPE)
  {
    if (!TYPE_IDENTIFIER(field)) type_name_s = "__anonymous_enum__";
    else type_name_s = get_type_name(field);
  }
  else
  {
    type_id = TYPE_IDENTIFIER(field);
    if (type_id) type_name_s = IDENTIFIER_POINTER(type_id);
    else type_name_s = "N/A";
  }

  sprintf(num_str, "%ld", field_size);

  add_indent(xml_str, indent_level);
  *xml_str = strapp(*xml_str, (char *)"<scalar size=\"");
  *xml_str = strapp(*xml_str, (char *)num_str);
  *xml_str = strapp(*xml_str, (char *)"\" type-name=\"");
  *xml_str = strapp(*xml_str, (char *)type_name_s);
  *xml_str = strapp(*xml_str, (char *)"\" unsigned=\"");
  *xml_str = strapp(*xml_str, (char *)(TYPE_UNSIGNED(field) ? "true" : "false"));
  *xml_str = strapp(*xml_str, (char *)"\"/>\n");
}

  /**
   *  @fn void add_c_decl(char **xml_str,
   *                      const_tree base_type,
   *                      const char *name,
   *                      int indent_level)
   *
   *  @brief adds struct, union, or enum element to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param base_type - GCC node, points to base type of declaration
   *  @param name - name of node variable
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_c_decl(char **xml_str,
                       const_tree base_type,
                       const char *name,
                       int indent_level)
{
  if (name)
  {
    if (was_dumped(name))  // this was already added
      goto exit;

    add_to_dumped_structs(name);
  }

  if (!COMPLETE_TYPE_P(base_type))
    goto exit;

  if (!is_struct(base_type) && !is_union(base_type) && !is_enum(base_type))
    goto exit;

  ++indent_level;

  if (is_struct(base_type))
    add_struct(xml_str, base_type, name, indent_level);
  else if (is_union(base_type))
    add_union(xml_str, base_type, name, indent_level);
  else if (is_enum(base_type))
    add_enum(xml_str, base_type, name, indent_level);

exit:
  return;
}

  /**
   *  @fn void add_user_type_reference(char **xml_str,
   *                                   const_tree base_type,
   *                                   const char *name,
   *                                   int indent_level)
   *
   *  @brief adds a user defined type reference to XML buffer
   *
   *  @param xml_str - address of XML string buffer to append to
   *  @param base_type - GCC node, points to base type of declaration
   *  @param name - name of node variable
   *  @param indent_level - number of indents to emit for each appended line
   *
   *  @par Returns
   *  Nothing.
   */
  
static void add_user_type_reference(char **xml_str,
                                    const_tree base_type,
                                    const char *name,
                                    int indent_level)
{
  add_indent(xml_str, indent_level);

  *xml_str = strapp(*xml_str, (char *)"<type-reference type=\"");

  if (is_struct(base_type))
    *xml_str = strapp(*xml_str, (char *)"struct");
  else if (is_union(base_type))
    *xml_str = strapp(*xml_str, (char *)"union");
  else if (is_enum(base_type))
    *xml_str = strapp(*xml_str, (char *)"enum");

  *xml_str = strapp(*xml_str, (char *)"\" name=\"");

  *xml_str = strapp(*xml_str, (char *)name);

  *xml_str = strapp(*xml_str, (char *)"\"/>\n");
}

  /**
   *  @fn void finish_type(void *event_data, void *user_data)
   *
   *  @brief called when type declaration is finished in GCC
   *
   *  @param event_data - pointer to GCC plugin event data (specific to each event)
   *  @param user_data - pointer to user supplied data when registered
   *
   *  @par Returns
   *  Nothing.
   */
  
static void finish_type(void *event_data, void *user_data)
{
  tree type = NULL_TREE;
  char **xml_str = NULL;
  const char *type_name_s = NULL;
  tree type_name;

  type = (tree)event_data;
  if (!type) return;

  xml_str = (char **)user_data;
  if (!xml_str) return;

    // if is not struct, union, or enum then check if it is a typedef
    // if it is a typedef and a name can be established, then proceed

  if (!is_struct_or_union_or_enum(type) || !TYPE_FIELDS(type))
  {
    if ((TREE_CODE(type) == TYPE_DECL) && (type = TREE_TYPE(type)))
    {
      type_name_s = get_type_name(type);
      if (!type_name_s) return;
    }
    else return;
  }

    // If a type name is not yet found

  if (!type_name_s)
  {
    type_name = TYPE_IDENTIFIER(type);
    if (!type_name) return;
    type_name_s = IDENTIFIER_POINTER(type_name);
  }

    // if a target is specified and this is not it, then bail out

  if (target && type_name_s && strcmp(type_name_s, target)) return;

    // if type declaration is not in the main input file, then bail out

  if (strcmp(LOCATION_FILE(input_location), main_input_filename)) return;

  add_c_decl(xml_str, type, type_name_s, 0);
}

  /**
   *  @fn void finish(void *event_data, void *user_data)
   *
   *  @brief called when GCC plugin is cleaning up
   *
   *  @param event_data - pointer to GCC plugin event data (specific to each event)
   *  @param user_data - pointer to user supplied data when registered
   *
   *  @par Returns
   *  Nothing.
   */
  
static void finish(void *event_data, void *user_data)
{
  char **xml_str = (char **)user_data;

    // all leftovers
  for (list *iter = to_dump._list.next; iter != NULL; iter = iter->next)
  {
    dump_list *n = container_of(iter, dump_list, _list);

      // if it's not complete by now, it must've had references only as a pointer
      // with a forward declaration.  by the time a struct field is declared,
      // the type must be complete

    if (COMPLETE_TYPE_P(n->type))
    {
        // items added to the list are surely named.
      if (TYPE_IDENTIFIER(n->type))
        add_c_decl(xml_str, n->type, get_type_name(n->type), 0);
    }
  }

  *xml_str = strapp(*xml_str, (char *)"</c-decls>\n");

  fputs(*xml_str, output_file);
  fflush(output_file);
}

  /**
   *  @fn int plugin_init(struct plugin_name_args *plugin_info,
   *                      struct plugin_gcc_version *version)
   *
   *  @brief entry point of GCC plugin, called when GCC plugin system
   *         is initializing
   *
   *  @param plugin_info - pointer to @a plugin_name_args struct
   *  @param version - pointer to @a plugin_gcc_version struct
   *
   *  @return 0 on success
   *         -1 on failure
   */
  
int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version)
{
  const char *output = NULL;
  char **xml_str = &_xml_str;

  if (!plugin_default_version_check(version, &gcc_version))
  {
    error("incompatible gcc/plugin versions");
    return 1;
  }

  set_indent(2);

  for (int i = 0; i < plugin_info->argc; ++i)
  {
      // can be given with -fplugin-arg-c_decls_to_xml-output=<output file>
    if (!strcmp(plugin_info->argv[i].key, "output"))
      output = xstrdup(plugin_info->argv[i].value);

      // can be given with -fplugin-arg-c_decls_to_xml-struct=<struct>
    if (!strcmp(plugin_info->argv[i].key, "struct"))
      target = xstrdup(plugin_info->argv[i].value);

      // can be given with -fplugin-arg-c_decls_to_xml-indent=<struct>
    if (!strcmp(plugin_info->argv[i].key, "indent"))
      set_indent(atoi(plugin_info->argv[i].value));
  }

  if (!output)
  {
    fprintf(stderr,
            "c_decls_to_xml plugin: missing parameter: "
            "-fplugin-arg-c_decls_to_xml-output=<output>\n");
    exit(EXIT_FAILURE);
  }

  output_file = fopen(output, "w");

  if (!output_file)
  {
    perror(output);
    exit(EXIT_FAILURE);
  }

  *xml_str = strapp(*xml_str, (char *)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  *xml_str = strapp(*xml_str, (char *)"<c-decls>\n");

  register_callback(plugin_info->base_name,
                    PLUGIN_FINISH_TYPE,
                    finish_type,
                    xml_str);

  register_callback(plugin_info->base_name,
                    PLUGIN_FINISH_DECL,
                    finish_type,
                    xml_str);

  register_callback(plugin_info->base_name,
                    PLUGIN_FINISH,
                    finish,
                    xml_str);

  return 0;
}

  /**
   *  @fn void set_indent(int indent)
   *
   *  @brief sets number of spaces to emit on each indent level.
   *
   *  @param indent - number of spaces
   *
   *  @par Returns
   *  Nothing.
   */
  
static void set_indent(int indent)
{
  if (indent < 0) indent = 0;
  _indent = indent;
}

static char *strapp(char *s1, char *s2)
{
  int len1, len2;
  char *tmp;

  if (!s1)
  {
    s1 = (char *)xmalloc(1);
    if (!s1) return NULL;
    *s1 = 0;
  }

  if (!s2) return s1;

  len1 = strlen(s1);
  len2 = strlen(s2);

  tmp = (char *)xrealloc(s1, len1 + len2 + 1);
  if (tmp)
  {
    s1 = tmp;
    strcat(s1, s2);
  }

  return s1;
}

