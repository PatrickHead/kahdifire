# kahdifire - C enum, struct and union code generator

An application, 'kahdifire' and a GCC plugin 'c_decls_to_xml.so', which transform C enum, struct and union declarations into code modules

<a id="TOC"></a>
## Table of contents

* [Introduction](#introduction)
* [Installation](#installation)
* [Usage](#usage)
* [Example](#example)
* [Issues and Limitations](#known-issues-and-limitations)
* [Getting Help](#getting-help)
* [License Terms](#license)

<a id="introduction"></a>
## Introduction

This software package contains two items of interest.  First is a GCC Plugin module that will extract all the enum, struct, and union declarations for a C language source code file, and subsequently produce an XML file that can be used by other applications to diagram, report on, or produce code from.  The second item is an application that can generate boiler plate code to create, duplicate, free and manage the contents of the enums, structs or unions as recorded in the XML file.

The GCC Plugin module is named <b>c_decls_to_xml.so</b>.
The code generating application is named <b>kahdifire</b>.  This name is a play on the word codify - to put documents or rules into code.  A mechanism that can codify programming rules or object definitions could be called a <i>codifier</i>.  Since this is probably a common name for other applications with similar purposes, and to help clarify the pronunciation of the name, the name <i>codifier</i> has been spelled more phonetically as <b>kahdifire</b>.

[Back to Table of Contents](#TOC)

<a id="installation"></a>
## Installation

A reasonably modern version of GNU Autotools should be installed before installing @b kahdifire from the source files.   If you have installed @b kahdifire from a ready made DPKG package, then the rest of these installation instructions do not apply.  At the time of writing this document, Autotools version 2.71 were used.

To compile the source code, the standard Debian build essentials packages are required.

This package requires that the libxml2 development library is installed.

If you want to create the Doxygen API documentation, then of course Doxygen and friends need to installed.

If you have obtained the @b kahdifire source code from a GIT repository, then execute the `autoreconf` program while in the directory that the @b dedup GIT clone is located.

For both GIT clones and GNU Package tarballs, run the usual `./configure` followed by `make`.

To install the built `kahdifire` utility and **libdedup** library, run `sudo make install`.

#### Summary Steps:
1. git clone <i><u>https://github.com/PatrickHead/kahdifire.git</u></i>
2. autoreconfigure
3. make  
4. sudo make install

The GCC Plugin Module will be installed in <i>/usr/local/lib/kahdifire</i>.  

The <b>kahdifire</b> application will be installed in <i>/usr/local/bin</i>.  

The code documentation will be installed in <i>/usr/local/share/doc/kahdifire/html</i>.

[Back to Table of Contents](#TOC)

<a id="usage"></a>
## Usage

    usage:

      kahdifire [-a <annotation>] [-b <base name>] [-l <license type>] [-m]
                [-M <makefile options>] [-r] [-g <generator options>]
                <input file>

      kahdifire -h

    where:

      <annotation> is of:
        none
        text
        doxygen

      <base name> is base name of project output files including directory path

      <license type> is one of:
        none
        gplv3
        lgplv3
        usalgplv3
        publicdomain

      <makefile options> is a comma separated list of:
        CC=<c compiler>
        COPTS=<compiler options>
        INSTALL_DIR=<base directory for installation>

      <generator options> is a comma separated list of:
        array - generate code for a dynamic array handler
        list - generate code for a doubly linked list handler
        avl - generate code for an AVL (balanced b-tree) handler

      <input file> is name of XML file containing C declarations

      -m = generate a makefile

      -r = generate a README.md file

      -h = this help display

[Back to Table of Contents](#TOC)

<a id="example"></a>
## Example

Assuming you have a file named <i>example-def.h</i> in your current working directory with the following contents:

```C
struct person
{
  unsigned int id;
  char *first_name;
  char *middle_name_or_initial;
  char *last_name;
};
```

If you run the following command:

<b>gcc -fplugin=modules/c_decls_to_xml.so -fplugin-arg-c_decls_to_xml-output=example-def.decls -fplugin-arg-c_decls_to_xml-struct=person -fplugin-arg-c_decls_to_xml-indent=2 -c example-def.h -o /dev/null</b>

will produce a file named <i>example-def.decls</i> containing:

```XML
<?xml version="1.0" encoding="UTF-8"?>
<c-decls>
  <struct name="person" size="256">
    <field name="id" offset="0">
      <scalar size="32" type-name="unsigned int" unsigned="true"/>
    </field>
    <field name="first_name" offset="64">
      <pointer size="64">
        <scalar size="8" type-name="char" unsigned="false"/>
      </pointer>
    </field>
    <field name="middle_name_or_initial" offset="128">
      <pointer size="64">
        <scalar size="8" type-name="char" unsigned="false"/>
      </pointer>
    </field>
    <field name="last_name" offset="192">
      <pointer size="64">
        <scalar size="8" type-name="char" unsigned="false"/>
      </pointer>
    </field>
  </struct>
</c-decls>
```

Then running the following command:

<b>bin/kahdifire -b example example-def.decls</b>

will produce two files;

<i>example.h</i> containing:

```C
#warning change YEAR, FIRSTNAME and LASTNAME, then remove this line

/*
 *  Copyright YEAR - FIRSTNAME LASTNAME
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/*
 *
 *  This file was generated by kahdifire
 */

#ifndef EXAMPLE_H
#define EXAMPLE_H

#warning find and replace all occurences of USER ANNOTATION, then remove this line

    /*
     *  Typedefs for all declared enums, structs and unions
     */

typedef struct person person;

    /*
     *  Declarations for all enums, structs and unions
     */

struct person
{
  unsigned int id;
  char *first_name;
  char *middle_name_or_initial;
  char *last_name;
};

    /*
     *  Function prototypes for all declared enums, structs and unions
     */

  /*
   *  Utility functions for struct person
   */

person *example_person_new(void);
person *example_person_dup(person *instance);
void example_person_free(person *instance);
unsigned int example_person_get_id(person *instance);
void example_person_set_id(person *instance, unsigned int id);
char *example_person_get_first_name(person *instance);
void example_person_set_first_name(person *instance, char *first_name);
char *example_person_get_middle_name_or_initial(person *instance);
void example_person_set_middle_name_or_initial(person *instance, char *middle_name_or_initial);
char *example_person_get_last_name(person *instance);
void example_person_set_last_name(person *instance, char *last_name);

#endif //EXAMPLE_H
```

and <i>example.c</i> containing:

```C
#warning change YEAR, FIRSTNAME and LASTNAME, then remove this line

/*
 *  Copyright YEAR - FIRSTNAME LASTNAME
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#warning find and replace all occurences of USER ANNOTATION, then remove this line

/*
 *
 *  This file was generated by kahdifire
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "example.h"

    /*
     *  Utility functions for struct person
     */

person *example_person_new(void)
{
  person *instance = NULL;

  instance = malloc(sizeof(person));
  if (instance) memset(instance, 0, sizeof(person));

  return instance;
}

person *example_person_dup(person *instance)
{
  person *new_instance = NULL;

  if (!instance) goto exit;

  new_instance = malloc(sizeof(person));
  if (!new_instance) goto exit;

  memcpy(new_instance, instance, sizeof(person));

  if (instance->first_name)
    new_instance->first_name = strdup(instance->first_name);

  if (instance->middle_name_or_initial)
    new_instance->middle_name_or_initial = strdup(instance->middle_name_or_initial);

  if (instance->last_name)
    new_instance->last_name = strdup(instance->last_name);

exit:
  return new_instance;
}

void example_person_free(person *instance)
{
  if (!instance) goto exit;

  if (instance->first_name)
    free(instance->first_name);

  if (instance->middle_name_or_initial)
    free(instance->middle_name_or_initial);

  if (instance->last_name)
    free(instance->last_name);

  free(instance);

exit:
}

unsigned int example_person_get_id(person *instance)
{
  return instance ? instance->id : 0;
}

void example_person_set_id(person *instance, unsigned int id)
{
  if (!instance) return;

  if (instance) instance->id = id;
}

char *example_person_get_first_name(person *instance)
{
  return instance ? instance->first_name : NULL;
}

void example_person_set_first_name(person *instance, char *first_name)
{
  if (!instance) return;

  if (instance->first_name) free(instance->first_name);
  instance->first_name = NULL;
  if (first_name) instance->first_name = strdup(first_name);
}

char *example_person_get_middle_name_or_initial(person *instance)
{
  return instance ? instance->middle_name_or_initial : NULL;
}

void example_person_set_middle_name_or_initial(person *instance, char *middle_name_or_initial)
{
  if (!instance) return;

  if (instance->middle_name_or_initial) free(instance->middle_name_or_initial);
  instance->middle_name_or_initial = NULL;
  if (middle_name_or_initial) instance->middle_name_or_initial = strdup(middle_name_or_initial);
}

char *example_person_get_last_name(person *instance)
{
  return instance ? instance->last_name : NULL;
}

void example_person_set_last_name(person *instance, char *last_name)
{
  if (!instance) return;

  if (instance->last_name) free(instance->last_name);
  instance->last_name = NULL;
  if (last_name) instance->last_name = strdup(last_name);
}

```

<a id="known-issues-and-limitations"></a>
## Known issues and limitations

As of 01/14/2025, this application is currently in beta testing and may contain bugs that make it not useful for production purposes.

[Back to Table of Contents](#TOC)

<a id="getting-help"></a>
## Getting help

If you encounter any bugs or require additional help, contact [Patrick Head](mailto:patrick.t.head.ctr@us.navy.mil)

[Back to Table of Contents](#TOC)

<a id="license"></a>
## License

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

_This_ README file is itself distributed under the terms of the [Creative Commons 1.0 Universal license (CC0)](https://creativecommons.org/publicdomain/zero/1.0/).

[Back to Table of Contents](#TOC)

