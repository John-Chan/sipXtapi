include $(top_srcdir)/config/sipXcommon.am.inc

bin_PROGRAMS = ReceiveCall

INCLUDES = -Iinclude -I../PlaceCall/include

ReceiveCall_LDADD = \
    $(top_builddir)/sipXcallLib/src/libsipXcall.la \
    @CA_LIBS@ \
    @SIPXMEDIA_MP_LIBS@ 

ReceiveCall_SOURCES = \
    src/ReceiveCall.cpp \
    ../PlaceCall/src/ExternalTransport.cpp

noinst_HEADERS = \
    include/ReceiveCall.h \
    include/ReceiveCallWntApp.h
