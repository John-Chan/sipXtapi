
INCLUDES = \
	-I$(top_srcdir)/sipXportLib/src/test \
	-I$(top_srcdir)/sipXportLib/include

lib_LTLIBRARIES = libsipXunit.la

libsipXunit_la_LDFLAGS = -static # no version info for static libs
libsipXunit_la_CFLAGS = @CPPUNIT_CFLAGS@
libsipXunit_la_CPPFLAGS = @CPPUNIT_CFLAGS@ 

libsipXunit_la_SOURCES =  \
    unitmain.cpp \
    TestUtilities.cpp \
    TestRunner.cpp \
    TestRunner.h \
    TestMonitor.cpp \
    TestMonitor.h \
    TestOutputter.cpp \
    TestOutputter.h \
    TestOsSysLogListener.cpp \
    TestOsSysLogListener.h \
    TestTemplate.cpp

libsipXunit_la_LIBADD = \
	@CPPUNIT_LIBS@

sipxincludedir = $(pkgincludedir)/sipxunit
sipxinclude_HEADERS = \
    TestUtilities.h


