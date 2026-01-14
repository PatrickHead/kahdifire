/*
 *  Copyright 2025,2026 Patrick T. Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 *  @file c_decls_to_xml.h
 *  @brief GCC Plugin to parse C source code to struct/union/enum declarations.
 *
 *  Output is in XML format
 */

#ifndef C_DECLS_TO_XML_H
#define C_DECLS_TO_XML_H

  /**
   *  @typedef list
   *  @brief creates a type for a @a list struct
   */

typedef struct list list;

  /**
   *  @struct list
   *  @brief defines a singly linked list
   *
   *  This is of no use in itself, but aids in other list types
   */

struct list {
  list *next;  /**< pointer to next item in list */
};

  /**
   *  @typedef dumped_list
   *  @brief creates a type for a @a dumped_list struct
   */

typedef struct dumped_list dumped_list;

  /**
   *  @struct dumped_list
   *  @brief defines individual element in linked list of already dumped items
   *
   *  This is of no use in itself, but aids in other list types
   */

struct dumped_list {
  list _list;    /**< singly linked list                */
  char name[0];  /**< name of item that has been dumped */
};

  /**
   *  @typedef dump_list
   *  @brief creates a type for a @a dump_list struct
   */

typedef struct dump_list dump_list;

  /**
   *  @struct dump_list
   *  @brief defines individual element in linked list of items to dump
   *
   *  This is of no use in itself, but aids in other list types
   */

struct dump_list {
  list _list;  /**< singly linked list              */
  tree type;   /**< pointer to GCC plugin tree type */
};

#endif //C_DECLS_TO_XML_H
