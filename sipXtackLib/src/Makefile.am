## Process this file with automake to produce Makefile.in

SUBDIRS = . test

## Make these part of optional compile under IF we use
## GNU tools on windows
EXTRA_DIST = \
    resparse/wnt/inet_addr.c \
    resparse/wnt/writev.c

INCLUDES = \
	-I$(top_srcdir)/sipXtackLib/include \
	-I$(top_builddir)/sipXtackLib/include \
        -I$(top_srcdir)/sipXsdpLib/include \
	-I$(top_srcdir)/sipXportLib/include

lib_LTLIBRARIES = libsipXtack.la

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=0
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

libsipXtack_la_LDFLAGS = \
	-version-info ${version_Current}:${version_Revision}:${version_Age}

libsipXtack_la_CXXFLAGS = \
	-DSIPX_LOGDIR=\"@SIPX_LOGDIR@\" \
	-DSIPX_CONFDIR=\"@SIPX_CONFDIR@\"

libsipXtack_la_LIBADD = \
	$(top_builddir)/sipXportLib/src/libsipXport.la \
        $(top_builddir)/sipXsdpLib/src/libsipXsdp.la \
	-lresolv -lstdc++

if BUILDTLS
SECURITY_FILES=\
    net/SipTlsServer.cpp
else
SECURITY_FILES=
endif

nodist_libsipXtack_la_SOURCES =  \
    $(top_srcdir)/config/@PACKAGE@-buildstamp.h \
    $(top_srcdir)/config/@PACKAGE@-buildstamp.cpp

libsipXtack_la_SOURCES =  \
    net/HttpBody.cpp \
    net/HttpMessage.cpp \
    net/HttpRequestContext.cpp \
    net/HttpServer.cpp \
    net/HttpService.cpp \
    net/MailAttachment.cpp \
    net/MailMessage.cpp \
    net/MimeBodyPart.cpp \
    net/NameValuePair.cpp \
    net/NameValuePairInsensitive.cpp \
    net/NetAttributeTokenizer.cpp \
    net/NetBase64Codec.cpp \
    net/NetMd5Codec.cpp \
    net/ProvisioningAgent.cpp \
    net/ProvisioningAgentXmlRpcAdapter.cpp \
    net/ProvisioningAttrList.cpp \
    net/ProvisioningClass.cpp \
    net/SdpBody.cpp \
    net/SdpHelper.cpp \
    net/SipClient.cpp \
    net/SipContactDb.cpp \
    net/SipConfigServerAgent.cpp \
    net/SipDialog.cpp \
    net/SipDialogEvent.cpp \
    net/SipDialogMgr.cpp \
    net/SipDialogMonitor.cpp \
    net/SipLine.cpp \
    net/SipLineCredentials.cpp \
    net/SipLineEvent.cpp \
    net/SipLineList.cpp \
    net/SipLineMgr.cpp \
    net/SipMessage.cpp \
    net/SipMessageEvent.cpp \
    net/SipMessageList.cpp \
    net/SipNonceDb.cpp \
    net/SipNotifyStateTask.cpp \
    net/SipObserverCriteria.cpp \
    net/SipPimClient.cpp \
    net/SipPresenceEvent.cpp \
    net/SipProtocolServerBase.cpp \
    net/SipPublishContentMgr.cpp \
    net/SipPublishServer.cpp \
    net/SipPublishServerEventStateCompositor.cpp \
    net/SipPublishServerEventStateMgr.cpp \
    net/SipRefreshMgr.cpp \
    net/SipRefreshManager.cpp \
    net/SipRequestContext.cpp \
    net/SipResourceList.cpp \
    net/SipServerBroker.cpp \
    net/SipServerBase.cpp \
    net/SipSession.cpp \
    net/SipSrvLookup.cpp \
    net/SipSubscribeClient.cpp \
    net/SipSubscribeServer.cpp \
    net/SipSubscribeServerEventHandler.cpp \
    net/SipSubscriptionMgr.cpp \
    net/SipTcpServer.cpp \
    net/SipTransaction.cpp \
    net/SipTransactionList.cpp \
    net/SipUdpServer.cpp \
    net/SipUserAgentBase.cpp \
    net/SipUserAgent.cpp \
    net/SipUserAgentStateless.cpp \
    net/SmimeBody.cpp \
    net/StateChangeNotifier.cpp \
    net/TapiMgr.cpp \
    net/Url.cpp \
    net/XmlRpcBody.cpp \
    net/XmlRpcDispatch.cpp \
    net/XmlRpcMethod.cpp \
    net/XmlRpcRequest.cpp \
    net/XmlRpcResponse.cpp \
    net/HttpConnection.cpp \
    net/HttpConnectionMap.cpp \
    net/PidfBody.cpp \
    resparse/bzero.c \
    resparse/memset.c \
    resparse/ns_name.c \
    resparse/ns_netint.c \
    resparse/res_comp.c  \
    resparse/res_copy.c \
    resparse/res_data.c \
    resparse/res_free.c \
    resparse/res_info.c \
    resparse/res_init.c \
    resparse/res_mkquery.c \
    resparse/res_parse.c \
    resparse/res_print.c \
    resparse/res_query.c \
    resparse/res_send.c \
    $(SECURITY_FILES)

