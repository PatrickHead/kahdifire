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
 *  @file doxygen.c
 *  @brief Converts C declarations to doxygen
 *
 *  Input in XML format as produced by c_decls_to_xml.so GCC plugin
 *
 *  Output is doxygen
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

#include "doxygen.h"
#include "options.h"

  /*  Module specific function prototypes  */

void emit_doxygen_configuration(FILE *outfile, char *project_name);

/**
 *  @fn void gen_doxygen_configuration(xmlDocPtr doc, char *base_name)
 *
 *  @brief generates doxygen configuration file
 *
 *  @param doc - xmlDocPtr containing declaration metadata
 *  @param base_name - basic name of project for output files
 *
 *  @par Returns
 *  Nothing.
 */

void gen_doxygen_configuration(xmlDocPtr doc, char *base_name)
{
  xmlNodePtr root;
  FILE *outfile = NULL;
  char *tmp;
  char *base_dir = NULL;
  char *outfile_name = NULL;
  char *project_name = NULL;

  if (!doc || !base_name) goto exit;

  if (option_annotation() != annotation_type_doxygen) goto exit;

  root = xmlDocGetRootElement(doc);
  if (!root) goto exit;

  if (!root->children) goto exit;

  if (strcmp((char *)root->name, "c-decls")) goto exit;

  tmp = strdup(base_name);
  if (!tmp) goto exit;

  base_dir = strdup(dirname(tmp));

  free(tmp);

  project_name = get_project_name(base_name);
  if (!project_name) goto exit;

  outfile_name = malloc(strlen(base_dir) +
                        strlen("Doxygen") +
                        strlen(project_name) +
                        3);
  if (!outfile_name) goto exit;

  sprintf(outfile_name, "%s/Doxygen.%s", base_dir, project_name);

  outfile = fopen(outfile_name, "w");
  if (!outfile) goto exit;

  emit_doxygen_configuration(outfile, project_name);

exit:
  if (outfile) fclose(outfile);
  if (outfile_name) free(outfile_name);
  if (project_name) free(project_name);
  if (base_dir) free(base_dir);
}

static char *_doxygen_config =  /**<  format string for doxygen configuration  */
  "DOXYFILE_ENCODING      = UTF-8\n"
  "PROJECT_NAME           = \"USER SUPPLIED NAME for project %s\"\n"
  "PROJECT_NUMBER         =\n"
  "PROJECT_BRIEF          = \"USER SUPPLIED BRIEF for project %s\"\n"
  "PROJECT_LOGO           =\n"
  "OUTPUT_DIRECTORY       = \"./doxygen\"\n"
  "CREATE_SUBDIRS         = NO\n"
  "ALLOW_UNICODE_NAMES    = NO\n"
  "OUTPUT_LANGUAGE        = English\n"
  "BRIEF_MEMBER_DESC      = YES\n"
  "REPEAT_BRIEF           = YES\n"
  "ABBREVIATE_BRIEF       = \"The $name class\" \\\n"
  "                         \"The $name widget\" \\\n"
  "                         \"The $name file\" \\\n"
  "                         is \\\n"
  "                         provides \\\n"
  "                         specifies \\\n"
  "                         contains \\\n"
  "                         represents \\\n"
  "                         a \\\n"
  "                         an \\\n"
  "                         the\n"
  "ALWAYS_DETAILED_SEC    = NO\n"
  "INLINE_INHERITED_MEMB  = NO\n"
  "FULL_PATH_NAMES        = NO\n"
  "STRIP_FROM_PATH        =\n"
  "STRIP_FROM_INC_PATH    =\n"
  "SHORT_NAMES            = NO\n"
  "JAVADOC_AUTOBRIEF      = NO\n"
  "JAVADOC_BANNER         = NO\n"
  "QT_AUTOBRIEF           = NO\n"
  "MULTILINE_CPP_IS_BRIEF = NO\n"
  "PYTHON_DOCSTRING       = YES\n"
  "INHERIT_DOCS           = YES\n"
  "SEPARATE_MEMBER_PAGES  = NO\n"
  "TAB_SIZE               = 2\n"
  "ALIASES                =\n"
  "OPTIMIZE_OUTPUT_FOR_C  = YES\n"
  "OPTIMIZE_OUTPUT_JAVA   = NO\n"
  "OPTIMIZE_FOR_FORTRAN   = NO\n"
  "OPTIMIZE_OUTPUT_VHDL   = NO\n"
  "OPTIMIZE_OUTPUT_SLICE  = NO\n"
  "EXTENSION_MAPPING      =\n"
  "MARKDOWN_SUPPORT       = YES\n"
  "TOC_INCLUDE_HEADINGS   = 5\n"
  "AUTOLINK_SUPPORT       = YES\n"
  "BUILTIN_STL_SUPPORT    = NO\n"
  "CPP_CLI_SUPPORT        = NO\n"
  "SIP_SUPPORT            = NO\n"
  "IDL_PROPERTY_SUPPORT   = NO\n"
  "DISTRIBUTE_GROUP_DOC   = NO\n"
  "GROUP_NESTED_COMPOUNDS = NO\n"
  "SUBGROUPING            = YES\n"
  "INLINE_GROUPED_CLASSES = NO\n"
  "INLINE_SIMPLE_STRUCTS  = NO\n"
  "TYPEDEF_HIDES_STRUCT   = NO\n"
  "LOOKUP_CACHE_SIZE      = 0\n"
  "NUM_PROC_THREADS       = 1\n"
  "EXTRACT_ALL            = NO\n"
  "EXTRACT_PRIVATE        = NO\n"
  "EXTRACT_PRIV_VIRTUAL   = NO\n"
  "EXTRACT_PACKAGE        = NO\n"
  "EXTRACT_STATIC         = YES\n"
  "EXTRACT_LOCAL_CLASSES  = YES\n"
  "EXTRACT_LOCAL_METHODS  = NO\n"
  "EXTRACT_ANON_NSPACES   = NO\n"
  "RESOLVE_UNNAMED_PARAMS = YES\n"
  "HIDE_UNDOC_MEMBERS     = NO\n"
  "HIDE_UNDOC_CLASSES     = NO\n"
  "HIDE_FRIEND_COMPOUNDS  = NO\n"
  "HIDE_IN_BODY_DOCS      = NO\n"
  "INTERNAL_DOCS          = NO\n"
  "CASE_SENSE_NAMES       = YES\n"
  "HIDE_SCOPE_NAMES       = NO\n"
  "HIDE_COMPOUND_REFERENCE= NO\n"
  "SHOW_INCLUDE_FILES     = YES\n"
  "SHOW_GROUPED_MEMB_INC  = NO\n"
  "FORCE_LOCAL_INCLUDES   = NO\n"
  "INLINE_INFO            = YES\n"
  "SORT_MEMBER_DOCS       = YES\n"
  "SORT_BRIEF_DOCS        = NO\n"
  "SORT_MEMBERS_CTORS_1ST = NO\n"
  "SORT_GROUP_NAMES       = NO\n"
  "SORT_BY_SCOPE_NAME     = NO\n"
  "STRICT_PROTO_MATCHING  = NO\n"
  "GENERATE_TODOLIST      = YES\n"
  "GENERATE_TESTLIST      = YES\n"
  "GENERATE_BUGLIST       = YES\n"
  "GENERATE_DEPRECATEDLIST= YES\n"
  "ENABLED_SECTIONS       =\n"
  "MAX_INITIALIZER_LINES  = 30\n"
  "SHOW_USED_FILES        = NO\n"
  "SHOW_FILES             = YES\n"
  "SHOW_NAMESPACES        = NO\n"
  "FILE_VERSION_FILTER    =\n"
  "LAYOUT_FILE            =\n"
  "CITE_BIB_FILES         =\n"
  "QUIET                  = YES\n"
  "WARNINGS               = YES\n"
  "WARN_IF_UNDOCUMENTED   = YES\n"
  "WARN_IF_DOC_ERROR      = YES\n"
  "WARN_NO_PARAMDOC       = YES\n"
  "WARN_AS_ERROR          = NO\n"
  "WARN_FORMAT            = \"$file:$line: $text\"\n"
  "WARN_LOGFILE           =\n"
  "INPUT                  = %s.h %s.c README.md\n"
  "INPUT_ENCODING         = UTF-8\n"
  "FILE_PATTERNS          = *.c \\\n"
  "                         *.cc \\\n"
  "                         *.cxx \\\n"
  "                         *.cpp \\\n"
  "                         *.c++ \\\n"
  "                         *.java \\\n"
  "                         *.ii \\\n"
  "                         *.ixx \\\n"
  "                         *.ipp \\\n"
  "                         *.i++ \\\n"
  "                         *.inl \\\n"
  "                         *.idl \\\n"
  "                         *.ddl \\\n"
  "                         *.odl \\\n"
  "                         *.h \\\n"
  "                         *.hh \\\n"
  "                         *.hxx \\\n"
  "                         *.hpp \\\n"
  "                         *.h++ \\\n"
  "                         *.cs \\\n"
  "                         *.d \\\n"
  "                         *.php \\\n"
  "                         *.php4 \\\n"
  "                         *.php5 \\\n"
  "                         *.phtml \\\n"
  "                         *.inc \\\n"
  "                         *.m \\\n"
  "                         *.markdown \\\n"
  "                         *.md \\\n"
  "                         *.mm \\\n"
  "                         *.dox \\\n"
  "                         *.py \\\n"
  "                         *.pyw \\\n"
  "                         *.f90 \\\n"
  "                         *.f95 \\\n"
  "                         *.f03 \\\n"
  "                         *.f08 \\\n"
  "                         *.f18 \\\n"
  "                         *.f \\\n"
  "                         *.for \\\n"
  "                         *.vhd \\\n"
  "                         *.vhdl \\\n"
  "                         *.ucf \\\n"
  "                         *.qsf \\\n"
  "                         *.ice\n"
  "RECURSIVE              = NO\n"
  "EXCLUDE                =\n"
  "EXCLUDE_SYMLINKS       = NO\n"
  "EXCLUDE_PATTERNS       =\n"
  "EXCLUDE_SYMBOLS        =\n"
  "EXAMPLE_PATH           =\n"
  "EXAMPLE_PATTERNS       = *\n"
  "EXAMPLE_RECURSIVE      = NO\n"
  "IMAGE_PATH             =\n"
  "INPUT_FILTER           =\n"
  "FILTER_PATTERNS        =\n"
  "FILTER_SOURCE_FILES    = NO\n"
  "FILTER_SOURCE_PATTERNS =\n"
  "USE_MDFILE_AS_MAINPAGE = README.md\n"
  "SOURCE_BROWSER         = YES\n"
  "INLINE_SOURCES         = NO\n"
  "STRIP_CODE_COMMENTS    = NO\n"
  "REFERENCED_BY_RELATION = NO\n"
  "REFERENCES_RELATION    = NO\n"
  "REFERENCES_LINK_SOURCE = YES\n"
  "SOURCE_TOOLTIPS        = YES\n"
  "USE_HTAGS              = NO\n"
  "VERBATIM_HEADERS       = YES\n"
  "CLANG_ASSISTED_PARSING = NO\n"
  "CLANG_ADD_INC_PATHS    = YES\n"
  "CLANG_OPTIONS          =\n"
  "CLANG_DATABASE_PATH    =\n"
  "ALPHABETICAL_INDEX     = YES\n"
  "IGNORE_PREFIX          =\n"
  "GENERATE_HTML          = YES\n"
  "HTML_OUTPUT            = html\n"
  "HTML_FILE_EXTENSION    = .html\n"
  "HTML_HEADER            =\n"
  "HTML_FOOTER            =\n"
  "HTML_STYLESHEET        =\n"
  "HTML_EXTRA_STYLESHEET  =\n"
  "HTML_EXTRA_FILES       =\n"
  "HTML_COLORSTYLE_HUE    = 210\n"
  "HTML_COLORSTYLE_SAT    = 100\n"
  "HTML_COLORSTYLE_GAMMA  = 80\n"
  "HTML_TIMESTAMP         = NO\n"
  "HTML_DYNAMIC_MENUS     = YES\n"
  "HTML_DYNAMIC_SECTIONS  = NO\n"
  "HTML_INDEX_NUM_ENTRIES = 100\n"
  "GENERATE_DOCSET        = NO\n"
  "DOCSET_FEEDNAME        = \"Doxygen generated docs\"\n"
  "DOCSET_BUNDLE_ID       = org.doxygen.Project\n"
  "DOCSET_PUBLISHER_ID    = org.doxygen.Publisher\n"
  "DOCSET_PUBLISHER_NAME  = Publisher\n"
  "GENERATE_HTMLHELP      = NO\n"
  "CHM_FILE               =\n"
  "HHC_LOCATION           =\n"
  "GENERATE_CHI           = NO\n"
  "CHM_INDEX_ENCODING     =\n"
  "BINARY_TOC             = NO\n"
  "TOC_EXPAND             = NO\n"
  "GENERATE_QHP           = NO\n"
  "QCH_FILE               =\n"
  "QHP_NAMESPACE          = org.doxygen.Project\n"
  "QHP_VIRTUAL_FOLDER     = doc\n"
  "QHP_CUST_FILTER_NAME   =\n"
  "QHP_CUST_FILTER_ATTRS  =\n"
  "QHP_SECT_FILTER_ATTRS  =\n"
  "QHG_LOCATION           =\n"
  "GENERATE_ECLIPSEHELP   = NO\n"
  "ECLIPSE_DOC_ID         = org.doxygen.Project\n"
  "DISABLE_INDEX          = NO\n"
  "GENERATE_TREEVIEW      = YES\n"
  "ENUM_VALUES_PER_LINE   = 1\n"
  "TREEVIEW_WIDTH         = 250\n"
  "EXT_LINKS_IN_WINDOW    = NO\n"
  "HTML_FORMULA_FORMAT    = png\n"
  "FORMULA_FONTSIZE       = 10\n"
  "FORMULA_TRANSPARENT    = YES\n"
  "FORMULA_MACROFILE      =\n"
  "USE_MATHJAX            = NO\n"
  "MATHJAX_FORMAT         = HTML-CSS\n"
  "MATHJAX_RELPATH        = https://cdn.jsdelivr.net/npm/mathjax@2\n"
  "MATHJAX_EXTENSIONS     =\n"
  "MATHJAX_CODEFILE       =\n"
  "SEARCHENGINE           = YES\n"
  "SERVER_BASED_SEARCH    = NO\n"
  "EXTERNAL_SEARCH        = NO\n"
  "SEARCHENGINE_URL       =\n"
  "SEARCHDATA_FILE        = searchdata.xml\n"
  "EXTERNAL_SEARCH_ID     =\n"
  "EXTRA_SEARCH_MAPPINGS  =\n"
  "GENERATE_LATEX         = NO\n"
  "LATEX_OUTPUT           = latex\n"
  "LATEX_CMD_NAME         =\n"
  "MAKEINDEX_CMD_NAME     = makeindex\n"
  "LATEX_MAKEINDEX_CMD    = makeindex\n"
  "COMPACT_LATEX          = NO\n"
  "PAPER_TYPE             = a4\n"
  "EXTRA_PACKAGES         =\n"
  "LATEX_HEADER           =\n"
  "LATEX_FOOTER           =\n"
  "LATEX_EXTRA_STYLESHEET =\n"
  "LATEX_EXTRA_FILES      =\n"
  "PDF_HYPERLINKS         = YES\n"
  "USE_PDFLATEX           = YES\n"
  "LATEX_BATCHMODE        = NO\n"
  "LATEX_HIDE_INDICES     = NO\n"
  "LATEX_BIB_STYLE        = plain\n"
  "LATEX_TIMESTAMP        = NO\n"
  "LATEX_EMOJI_DIRECTORY  =\n"
  "GENERATE_RTF           = NO\n"
  "RTF_OUTPUT             = rtf\n"
  "COMPACT_RTF            = NO\n"
  "RTF_HYPERLINKS         = NO\n"
  "RTF_STYLESHEET_FILE    =\n"
  "RTF_EXTENSIONS_FILE    =\n"
  "GENERATE_MAN           = NO\n"
  "MAN_OUTPUT             = man\n"
  "MAN_EXTENSION          = .3\n"
  "MAN_SUBDIR             =\n"
  "MAN_LINKS              = YES\n"
  "GENERATE_XML           = NO\n"
  "XML_OUTPUT             = xml\n"
  "XML_PROGRAMLISTING     = YES\n"
  "XML_NS_MEMB_FILE_SCOPE = NO\n"
  "GENERATE_DOCBOOK       = NO\n"
  "DOCBOOK_OUTPUT         = docbook\n"
  "GENERATE_AUTOGEN_DEF   = NO\n"
  "GENERATE_PERLMOD       = NO\n"
  "PERLMOD_LATEX          = NO\n"
  "PERLMOD_PRETTY         = YES\n"
  "PERLMOD_MAKEVAR_PREFIX =\n"
  "ENABLE_PREPROCESSING   = YES\n"
  "MACRO_EXPANSION        = NO\n"
  "EXPAND_ONLY_PREDEF     = NO\n"
  "SEARCH_INCLUDES        = YES\n"
  "INCLUDE_PATH           =\n"
  "INCLUDE_FILE_PATTERNS  =\n"
  "PREDEFINED             =\n"
  "EXPAND_AS_DEFINED      =\n"
  "SKIP_FUNCTION_MACROS   = YES\n"
  "TAGFILES               =\n"
  "GENERATE_TAGFILE       =\n"
  "ALLEXTERNALS           = NO\n"
  "EXTERNAL_GROUPS        = YES\n"
  "EXTERNAL_PAGES         = YES\n"
  "DIA_PATH               =\n"
  "HIDE_UNDOC_RELATIONS   = YES\n"
  "HAVE_DOT               = YES\n"
  "DOT_NUM_THREADS        = 0\n"
  "DOT_FONTNAME           = Helvetica\n"
  "DOT_FONTSIZE           = 10\n"
  "DOT_FONTPATH           =\n"
  "CLASS_GRAPH            = YES\n"
  "COLLABORATION_GRAPH    = YES\n"
  "GROUP_GRAPHS           = YES\n"
  "UML_LOOK               = YES\n"
  "UML_LIMIT_NUM_FIELDS   = 10\n"
  "DOT_UML_DETAILS        = YES\n"
  "DOT_WRAP_THRESHOLD     = 17\n"
  "TEMPLATE_RELATIONS     = NO\n"
  "INCLUDE_GRAPH          = YES\n"
  "INCLUDED_BY_GRAPH      = YES\n"
  "CALL_GRAPH             = YES\n"
  "CALLER_GRAPH           = YES\n"
  "GRAPHICAL_HIERARCHY    = YES\n"
  "DIRECTORY_GRAPH        = YES\n"
  "DOT_IMAGE_FORMAT       = png\n"
  "INTERACTIVE_SVG        = NO\n"
  "DOT_PATH               =\n"
  "DOTFILE_DIRS           =\n"
  "MSCFILE_DIRS           =\n"
  "DIAFILE_DIRS           =\n"
  "PLANTUML_JAR_PATH      =\n"
  "PLANTUML_CFG_FILE      =\n"
  "PLANTUML_INCLUDE_PATH  =\n"
  "DOT_GRAPH_MAX_NODES    = 75\n"
  "MAX_DOT_GRAPH_DEPTH    = 0\n"
  "DOT_TRANSPARENT        = NO\n"
  "DOT_MULTI_TARGETS      = NO\n"
  "GENERATE_LEGEND        = YES\n"
  "DOT_CLEANUP            = YES\n";

/**
 *  @fn void emit_doxygen_configuration(FILE *outfile, char *project_name)
 *
 *  @brief outputs Doxygen configuration
 *
 *  @param outfile - FILE * open for writing
 *  @param project_name - string containing name of project for rule names
 *
 *  @par Returns
 *  Nothing.
 */

void emit_doxygen_configuration(FILE *outfile, char *project_name)
{
  fprintf(outfile,
          _doxygen_config,
          project_name,
          project_name,
          project_name,
          project_name);
}

