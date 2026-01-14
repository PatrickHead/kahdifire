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
 *  @file annotation.h
 *  @brief tracks code generation annotation setting
 */

#ifndef ANNOTATION_H
#define ANNOTATION_H

  /**
   *  @typedef enum annotation_type
   *  @brief types of annotation that kahdifier can produce in source code
   */

typedef enum
{
  annotation_type_none = 0,  /**<  do not emit annotations           */
  annotation_type_text,      /**<  emit regular text annotations     */
  annotation_type_doxygen    /**<  emit regular doxygen annotations  */
} annotation_type;

annotation_type annotation_get_type(void);
void annotation_set_type(annotation_type type);

annotation_type annotation_string_to_type(char *string);
char *annotation_type_to_string(annotation_type type);

#endif //ANNOTATION_H

