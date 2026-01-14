AM_CPPFLAGS = -I $(srcdir)/include

AM_CFLAGS = -O3 -g0 -Wall

ARFLAGS = cr

PLUGIN_INCLUDE = @GCC_PLUGIN_INCLUDE@

pkglib_LTLIBRARIES = c_decls_to_xml.la
c_decls_to_xml_la_SOURCES = src/c_decls_to_xml.cpp src/strapp.c
c_decls_to_xml_la_CXXFLAGS = -O3 -g0 -Wall -I$(PLUGIN_INCLUDE) -fpic -shared -fno-inline-functions
c_decls_to_xml_la_LDFLAGS = -module

# is the following required?
#noinst_PROGRAMS =

bin_PROGRAMS = bin/kahdifire
bin_kahdifire_SOURCES = src/annotation.c src/common.c src/doxygen.c src/header-array.c src/header-avl.c src/header-list.c src/header.c src/kahdifire.c src/license.c src/makefile.c src/options.c src/readme.c src/source-array.c src/source-avl.c src/source-list.c src/source.c src/strapp.c
bin_kahdifire_CFLAGS = -O3 -g0 -Wall $(LIBXML2_CFLAGS) -fno-inline-functions
bin_kahdifire_LDADD = $(LIBXML2_LIBS)

EXTRA_DIST = examples man acdoxygen.m4 amdoxygen.am doxygen.kahdifier.cfg .gitignore .gitattributes

man1_MANS = man/kahdifire.1
dist_man_man1_MANS = man/kahdifire.1

super-clean: clean distclean
	@find . -name 'Makefile.in' -delete
	@find . -name 'config.h.in' -delete
	@find . -name '*.tar' -delete
	@find . -name '*.tar.*' -delete
	@find . -name '*~' -delete
	@rm -rf autom4te.cache
	@rm -f aclocal.m4
	@rm -f configure
	@rm -rf src/.deps
	@rm -f texput.log

include amdoxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)

EXTRA_DIST += $(DX_CONFIG)

#install-exec-hook:
#	$(INSTALL) -m 0755 c_decls_to_xml.so $(DESTDIR)$(libdir)

install-data-hook:
	@if test -d $(DX_HTML_DIR); \
	 then \
	   echo installing $(DX_HTML_DIR) to $(prefix)/share/doc/kahdifire; \
	   mkdir -p $(prefix)/share/doc/kahdifire; \
	   cp -r $(DX_HTML_DIR) $(prefix)/share/doc/kahdifire; \
	 fi

uninstall-hook:
	@rm -rf $(prefix)/share/doc/kahdifire
	@rm -f $(prefix)/lib/c_decls_to_xml.so

code-stats:
	@echo -n " total lines of source code: "; wc -l src/*.[ch] include/*.[ch] | tail -1 | sed -e 's/ total//' -e 's/^  *//'
	@echo -n "actual lines of source code: "; (gcc -fpreprocessed -dD -E src/*.[ch] include/*.[ch] | sed -e '/^$$/d' -e '/^# [0-9]/d' | wc -l)

