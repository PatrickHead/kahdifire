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
 *  along with this program. If not, see <https://www.gnu.org/license/>.
 */

/**
 *  @file license.h
 *  @brief tracks code generation license setting
 */

#ifndef LICENSE_H
#define LICENSE_H

  /**
   *  @typedef enum license_type
   *  @brief types of license that kahdifier can produce in source code
   */

typedef enum
{
  license_type_none = 0,      /**<  no license                  */
  license_type_GPL_v3,        /**<  GPL v3                      */
  license_type_LGPL_v3,       /**<  LGPL v3                     */
  license_type_USA_LGPL_v3,   /**<  US government + LGPL v3     */
  license_type_public_domain  /**<  public domain, non-license  */
} license_type;

license_type license_get_type(void);
void license_set_type(license_type type);

license_type license_string_to_type(char *string);
char *license_type_to_string(license_type type);

char *license_get_text(license_type type);

void license_emit(FILE *outfile);

#endif //LICENSE_H

