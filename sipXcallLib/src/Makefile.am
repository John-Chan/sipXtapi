## Process this file with automake to produce Makefile.in


EXTRA_DIST = tapi

SUBDIRS = . test

#INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/../sipXmediaAdapterLib/sipXmediaVoiceEngine
INCLUDES = -I$(top_srcdir)/sipXcallLib/include -I$(top_srcdir)/sipXmediaAdapterLib/sipXmediaMediaProcessing/include -I$(top_srcdir)/sipXmediaAdapterLib/interface -I$(top_srcdir)/sipXmediaLib/include -I$(top_srcdir)/sipXtackLib/include -I$(top_srcdir)/sipXsdpLib/include -I$(top_srcdir)/sipXportLib/include

lib_LTLIBRARIES = libsipXcall.la

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=0
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

libsipXcall_la_LDFLAGS = -version-info ${version_Current}:${version_Revision}:${version_Age}

libsipXcall_la_LIBADD = \
	$(top_builddir)/sipXmediaAdapterLib/sipXmediaMediaProcessing/src/libsipXmediaProcessing.la \
	$(top_builddir)/sipXtackLib/src/libsipXtack.la \
	-lstdc++

libsipXcall_la_CXXFLAGS = \
	-DSIPX_CONFDIR=\"@SIPX_CONFDIR@\" \
    @VIDEO_DEFINE@

nodist_libsipXcall_la_SOURCES = \
    $(top_srcdir)/config/@PACKAGE@-buildstamp.h \
    $(top_srcdir)/config/@PACKAGE@-buildstamp.cpp

libsipXcall_la_SOURCES = \
    cp/CallManager.cpp \
    cp/CSeqManager.cpp \
    cp/Connection.cpp \
    cp/CpCall.cpp \
    cp/CpCallManager.cpp \
    cp/CpGatewayManager.cpp \
    cp/CpGhostConnection.cpp \
    cp/CpIntMessage.cpp \
    cp/CpMultiStringMessage.cpp \
    cp/CpPeerCall.cpp \
    cp/CpStringMessage.cpp \
    cp/SipConnection.cpp \
    tao/TaoEvent.cpp \
    tao/TaoEventListener.cpp \
    tao/TaoListenerEventMessage.cpp \
    tao/TaoMessage.cpp \
    tao/TaoObjectMap.cpp \
    tao/TaoReference.cpp \
    tao/TaoString.cpp \
    tapi/sipXtapi.cpp \
    tapi/SipXEventDispatcher.cpp \
    tapi/sipXtapiEvents.cpp \
    tapi/sipXtapiInternal.cpp \
    tapi/SipXHandleMap.cpp \
    tapi/SipXMessageObserver.cpp

