include $(top_srcdir)/config/sipXcommon.am.inc

bin_PROGRAMS = PlaceCall

INCLUDES = -Iinclude

PlaceCall_LDADD = \
	$(top_builddir)/sipXcallLib/src/libsipXcall.la \
    @SIPXMEDIA_MP_LIBS@ \
    @CA_LIBS@
#    @SIPXMEDIA_VE_LIBS@  \
#    @CA_LIBS@

PlaceCall_SOURCES = \
    src/PlaceCall.cpp \
    src/ExternalTransport.cpp

noinst_HEADERS = \
    include/PlaceCall.h \
    include/PlaceCallWntApp.h \
    include/ExternalTransport.h
