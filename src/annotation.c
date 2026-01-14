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
 *  @file annotation.c
 *  @brief module to track annotation setting during code generation
 */

#include <string.h>

#include "config.h"

#include "annotation.h"

static annotation_type _type = annotation_type_none;  /**<  contains current annotation setting  */

  /**
   *  @fn annotation_type annotation_get_type(void)
   *
   *  @brief returns current annotation setting
   *
   *  @par Parameters
   *  None.
   *
   *  @returns annotation_type
   */
  
annotation_type annotation_get_type(void) { return _type; }

  /**
   *  @fn void annotation_set_type(annotation_type type)
   *
   *  @brief sets current annotation type
   *
   *  @param type - an annotation_type value
   *
   *  @par Returns
   *  Nothing.
   */
  
void annotation_set_type(annotation_type type) { _type = type; }

  /**
   *  @fn annotation_type annotation_string_to_type(char *string)
   *
   *  @brief returns @a annotation_type from @p string value
   *
   *  @param string - string name of annotation type
   *
   *  @returns annotation_type
   */
  
annotation_type annotation_string_to_type(char *string)
{
  annotation_type type = annotation_type_none;

  if (!string) goto exit;

  if (!strcasecmp(string, "text")) type = annotation_type_text;
  else if (!strcasecmp(string, "doxygen")) type = annotation_type_doxygen;

exit:
  return type;
}

  /**
   *  @fn char *annotation_type_to_string(annotation_type type)
   *
   *  @brief returns string name for @p type
   *
   *  @param type - annotation_type to convert to string
   *
   *  @returns string containing name of annotation_type
   */
  
char *annotation_type_to_string(annotation_type type)
{
  switch (type)
  {
    case annotation_type_none: return "none";
    case annotation_type_text: return "text";
    case annotation_type_doxygen: return "doxygen";
  }

  return "none";
}

