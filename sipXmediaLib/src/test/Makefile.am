INCLUDES = -I$(top_srcdir)/sipXmediaLib/include

TESTS = testsuite

check_PROGRAMS = testsuite sandbox

testsuite_LDADD = \
    $(top_builddir)/sipXportLib/src/test/sipxportunit/libsipXunit.la \
    $(CPPUNIT_LIBS) \
    $(top_builddir)/sipXportLib/src/libsipXport.la \
    $(top_builddir)/sipXtackLib/src/libsipXtack.la \
    @SPEEXDSP_LIBS@ \
    ../libsipXmedia.la \
    $(GIPS_OBJS)

testsuite_CXXFLAGS = -I$(SIPXUNITINC) \
                     @CPPUNIT_CFLAGS@

testsuite_CPPFLAGS = @CPPUNIT_CFLAGS@
    
# NOTE: Several media releated units tests are located in
## sipXcallLib because they made references to CallManager
testsuite_SOURCES = \
    ../../../sipXportLib/src/test/os/UnitTestLogHooks.cpp \
    ../../../sipXtackLib/src/net/NetBase64Codec.cpp \
    mp/dtmf5_48khz_16b_signed.h \
    mp/MpAudioBufTest.cpp \
    mp/MpCodecsPerformanceTest.cpp \
    mp/MpMediaTaskTest.cpp \
    mp/MpFlowGraphTest.cpp \
    mp/MpResourceTest.cpp \
    mp/MpResourceTopologyTest.cpp \
    mp/MpTestResource.h \
    mp/MpTestResource.cpp \
    mp/MpGenericResourceTest.h \
    mp/MpGenericResourceTest.cpp \
    mp/MprBridgeTest.cpp \
    mp/MprBridgeTestWB.cpp \
    mp/MprDelayTest.cpp \
    mp/MprFromFileTest.cpp \
    mp/MprFromMicTest.cpp \
    mp/MprMixerTest.cpp \
    mp/MprSpeakerSelectorTest.cpp \
    mp/MprSplitterTest.cpp \
    mp/MprToSpkrTest.cpp \
    mp/MprToneGenTest.cpp \
    mp/MpBufTest.cpp \
    mp/MpOutputDriverTest.cpp \
    mp/MpOutputManagerTest.cpp \
    mp/MpMMTimerTest.cpp \
    mp/MpWBInputOutputDeviceTest.cpp


# [DEBUG ASSISTANT] List a single unittest to isoloate a unittest for
# debugging or when developing a new unittest. As courtesy remove all
# unittests before checking in, although forgetting to has no ill
# effects. To build and run:
#
#   cd src/test
#   make sandbox
#   ./sandbox
#

sandbox_CXXFLAGS = $(testsuite_CXXFLAGS)

sandbox_CPPFLAGS = $(testsuite_CPPFLAGS)

sandbox_LDFLAGS = $(testsuite_LDFLAGS)

sandbox_LDADD = $(testsuite_LDADD)

sandbox_SOURCES = \
    ../../../sipXportLib/src/test/os/UnitTestLogHooks.cpp \
    ../../../sipXtackLib/src/net/NetBase64Codec.cpp \
    mp/RtcpParserTest.cpp 
