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
 *  @file kahdifire.c
 *  @brief Converts C declarations to module code and header
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

#include "kahdifire.h"
#include "options.h"

  /*
   *  Prototypes for functions in this module
   */

void usage(void);

  /**
   *  @fn int main(int argc, char **argv)
   *
   *  @brief Main entry point function for @b kahdifire application
   *
   *  @param argc - number of command line arguments
   *  @param argv - array of command line arguments
   *
   *  @return exist status, 0 on success
   */
  
int main(int argc, char **argv)
{
  int c;
  char *base_name = NULL;
  char *input_name = NULL;
  int retval = 0;

  while ((c = getopt(argc, argv, "b:a:l:g:hmM:r")) != EOF)
  {
    switch (c)
    {
      case 'r':
        option_gen_readme_on();
        break;

      case 'm':
        option_gen_makefile_on();
        break;

      case 'M':
        option_set_makefile_options(optarg);
        break;

      case 'a':
        option_set_annotation(optarg);
        break;

      case 'b':
        base_name = strdup(optarg);
        break;

      case 'l':
        option_set_license(optarg);
        break;

      case 'g':
        option_set_generator_options(optarg);
        break;

      case 'h':
      default:
        usage();
        goto exit;
        break;
    }
  }

  if (optind >= argc)
  {
    usage();
    goto exit;
  }

  input_name = strdup(argv[optind]);
  if (!input_name) goto exit;

  if (!base_name) base_name = create_base_name(input_name);
  if (!base_name) goto exit;

  retval = gen_code(input_name, base_name);

exit:
  if (base_name) free(base_name);
  if (input_name) free(input_name);

  return retval;
}

  /**
   *  @fn void usage(void)
   *
   *  @brief displays help message to user
   *
   *  @param prog - string containing name of application
   *
   *  @par Returns
   *  Nothing.
   */
  
void usage(void)
{
  printf("\n");
  printf("kahdifire version %s\n", VERSION);
  printf("\n");
  printf("  usage:\n");
  printf("\n");
  printf("    kahdifire [-a <annotation>] [-b <base name>] [-l <license type>] "
         "[-m]\n"
         "              [-M <makefile options>] [-r] [-g <generator options>]\n" 
         "              <input file>\n");
  printf("\n");
  printf("    kahdifire -h\n");
  printf("\n");
  printf("  where:\n");
  printf("\n");
  printf("    <annotation> is of:\n");
  printf("      none\n");
  printf("      text\n");
  printf("      doxygen\n");
  printf("\n");
  printf("    <base name> is base name of project output files "
         "including directory path\n");
  printf("\n");
  printf("    <license type> is one of:\n");
  printf("      none\n");
  printf("      gplv3\n");
  printf("      lgplv3\n");
  printf("      usalgplv3\n");
  printf("      publicdomain\n");
  printf("\n");
  printf("    <makefile options> is a comma separated list of:\n");
  printf("      CC=<c compiler>\n");
  printf("      COPTS=<compiler options>\n");
  printf("      INSTALL_DIR=<base directory for installation>\n");
  printf("\n");
  printf("    <generator options> is a comma separated list of:\n");
  printf("      array - generate code for a dynamic array handler\n");
  printf("      list - generate code for a doubly linked list handler\n");
  printf("      avl - generate code for an AVL (balanced b-tree) handler\n");
  printf("\n");
  printf("    <input file> is name of XML file containing C declarations\n");
  printf("\n");
  printf("    -m = generate a makefile\n");
  printf("\n");
  printf("    -r = generate a README.md file\n");
  printf("\n");
  printf("    -h = this help display\n");
  printf("\n");
}

