
INCLUDES = \
	-I$(top_srcdir)/sipXportLib/src/test \
	-I$(top_srcdir)/sipXportLib/include

lib_LTLIBRARIES = libsipXunit.la

libsipXunit_la_LDFLAGS = -static # no version info for static libs
libsipXunit_la_CFLAGS = @CPPUNIT_CFLAGS@
libsipXunit_la_CPPFLAGS = @CPPUNIT_CFLAGS@

libsipXunit_la_SOURCES =  \
    SipxPortUnitTestClass.cpp \
    SipxPortUnitTestConstructor.cpp \
    SipxPortUnitTestEnvironment.cpp \
    SipxPortUnitTestPointFailure.cpp \
    main.cpp \
    ../sipxunit/TestUtilities.cpp

libsipXunit_la_LIBADD = \
	@CPPUNIT_LIBS@

sipxincludedir = $(includedir)/sipxportunit
sipxinclude_HEADERS = \
	com_sipez_unit_UnitTestActivity.h \
	SipxPortUnitTestAsserts.h \
	SipxPortUnitTestClass.h \
	SipxPortUnitTestConstructor.h \
	SipxPortUnitTestEnvironment.h \
	SipxPortUnitTest.h \
	SipxPortUnitTestPointFailure.h


