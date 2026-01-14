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
 *  @file license.c
 *  @brief tracks code generation license setting
 */

#include <stdio.h>
#include <string.h>

#include "config.h"

#include "license.h"

static license_type _type = license_type_GPL_v3;  /**<  contains current license setting  */

  /**
   *  @fn license_type license_get_type(void)
   *
   *  @brief returns current license setting
   *
   *  @par Parameters
   *  None.
   *
   *  @returns license_type
   */
  
license_type license_get_type(void) { return _type; }

  /**
   *  @fn void license_set_type(license_type type)
   *
   *  @brief sets current license type
   *
   *  @param type - an license_type value
   *
   *  @par Returns
   *  Nothing.
   */
  
void license_set_type(license_type type) { _type = type; }

  /**
   *  @fn license_type license_string_to_type(char *string)
   *
   *  @brief returns @a license_type from @p string value
   *
   *  @param string - string name of license type
   *
   *  @returns license_type
   */
  
license_type license_string_to_type(char *string)
{
  license_type type = license_type_none;

  if (!string) goto exit;

  if (!strcasecmp(string, "gplv3")) type = license_type_GPL_v3;
  else if (!strcasecmp(string, "lgplv3")) type = license_type_LGPL_v3;
  else if (!strcasecmp(string, "usalgplv3")) type = license_type_USA_LGPL_v3;
  else if (!strcasecmp(string, "publicdomain")) type = license_type_public_domain;

exit:
  return type;
}

  /**
   *  @fn char *license_type_to_string(license_type type)
   *
   *  @brief returns string name for @p type
   *
   *  @param type - license_type to convert to string
   *
   *  @returns string containing name of license_type
   */
  
char *license_type_to_string(license_type type)
{
  switch (type)
  {
    case license_type_none: return "none";
    case license_type_GPL_v3: return "gplv3";
    case license_type_LGPL_v3: return "lgplv3";
    case license_type_USA_LGPL_v3: return "usalgplv3";
    case license_type_public_domain: return "publicdomain";
  }

  return "none";
}

  /**
   *  @fn char *license_get_text(license_type type);
   *
   *  @brief returns license text for @p type
   *
   *  @param type - license_type of text to return
   *
   *  @returns string containing text of @p type license
   */

#include "gplv3.txt.h"
#include "lgplv3.txt.h"
#include "usalgplv3.txt.h"
#include "publicdomain.txt.h"
  
char *license_get_text(license_type type)
{
  switch (type)
  {
    case license_type_none: return NULL;
    case license_type_GPL_v3: return _gplv3;
    case license_type_LGPL_v3: return _lgplv3;
    case license_type_USA_LGPL_v3: return _usalgplv3;
    case license_type_public_domain: return _publicdomain;
  }

  return NULL;
}

  /**
   *  @fn void license_emit(FILE *outfile)
   *
   *  @brief outputs the currently selected license text
   *
   *  @param outfile - open FILE * for output
   *
   *  @par Returns
   *  Nothing.
   */

void license_emit(FILE *outfile)
{
  char *license_text = NULL;

  if (!outfile) outfile = stdout;

  license_text = license_get_text(license_get_type());
  if (!license_text) goto exit;

  fputs(license_text, outfile);
  fputs("\n", outfile);

exit:
}

