//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDED_NETINTASK_H /* [ */
#define _INCLUDED_NETINTASK_H

#include "rtcp/RtcpConfig.h"

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "os/OsTask.h"
#include "os/OsLock.h"
#include "os/OsSocket.h"
#include "os/OsRWMutex.h"
#include "mp/MpTypes.h"
#include "mp/MpBuf.h"
#include "mp/MpMisc.h"

class OsNotification;

// DEFINES
#define NETWORK_MTU 1500
#define RTP_MTU  (NETWORK_MTU-12)   /// Network MTU minus RTP header size.
#define RTCP_MTU (NETWORK_MTU)

#define CODEC_TYPE_PCMU 0
#define CODEC_TYPE_PCMA 8
#define CODEC_TYPE_L16  11
#define CODEC_TYPE_SPEEX    110
#define CODEC_TYPE_SPEEX_5  111
#define CODEC_TYPE_SPEEX_15 112
#define CODEC_TYPE_SPEEX_24 113

#define RTP_DIR_IN  1
#define RTP_DIR_OUT 2
#define RTP_DIR_NEW 4

#define RTCP_DIR_IN  1
#define RTCP_DIR_OUT 2
#define RTCP_DIR_NEW 4

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS

class OsConnectionSocket;
class OsServerSocket;
class OsSocket;

struct rtpSession {
        UCHAR vpxcc; ///< Usually: ((2<<6) | (0<<5) | (0<<4) | 0)
        UCHAR mpt;   ///< Usually: ((0<<7) | 0)
        USHORT seq;
        UINT timestamp;
        UINT ssrc;
        OsSocket* socket;
        int dir;
        UINT packets;
        UINT octets;
        USHORT cycles;
};

#ifndef INCLUDE_RTCP /* [ */
struct __MprRtcpStats {
   UINT ssrc;
   short seqNumCycles;
   USHORT highSeqNum;
};

// TYPEDEFS

typedef struct __MprRtcpStats  MprRtcpStats;
typedef struct __MprRtcpStats* MprRtcpStatsPtr;
#endif /* INCLUDE_RTCP ] */

typedef struct rtpSession *rtpHandle;

// FORWARD DECLARATIONS
class MprFromNet;

extern UINT rand_timer32(void);
extern rtpHandle StartRtpSession(OsSocket* socket, int direction, char type);
extern void FinishRtpSession(rtpHandle h);

extern OsStatus startNetInTask();
extern OsStatus shutdownNetInTask();
extern OsStatus addNetInputSources(OsSocket* pRtpSocket,
            OsSocket* pRtcpSocket, MprFromNet* fwdTo, OsNotification* note);
extern OsStatus removeNetInputSources(MprFromNet* fwdTo, OsNotification* note);

class NetInTask : public OsTask
{

/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

   static const int DEF_NET_IN_TASK_PRIORITY;      ///< default task priority
   static const int DEF_NET_IN_TASK_OPTIONS;       ///< default task options
   static const int DEF_NET_IN_TASK_STACKSIZE;     ///< default task stacksize

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// Return a pointer to the NetIn task, creating it if necessary
   static NetInTask* getNetInTask();

     /// Destructor
   virtual
   ~NetInTask();

   int getWriteFD();
   
   
   void shutdownSockets();   

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

   virtual int run(void* pArg);

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

   OsConnectionSocket* getWriteSocket(void);
   OsConnectionSocket* getReadSocket(void);
   void openWriteFD(void);
   static OsRWMutex& getLockObj() { return sLock; }

//@}

/* ============================ INQUIRY =================================== */
/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

     /// Default constructor
   NetInTask(
      int prio    = DEF_NET_IN_TASK_PRIORITY,      ///< default task priority
      int options = DEF_NET_IN_TASK_OPTIONS,       ///< default task options
      int stack   = DEF_NET_IN_TASK_STACKSIZE);    ///< default task stacksize

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

   // Static data members used to enforce Singleton behavior
   static NetInTask* spInstance;    ///< pointer to the single instance of
                                    ///<  the MpNetInTask class
   static OsRWMutex     sLock;      ///< semaphore used to ensure that there
                                    ///<  is only one instance of this class

   OsConnectionSocket* mpWriteSocket;
   OsConnectionSocket* mpReadSocket;
   int               mCmdPort;      ///< internal socket port number

     /// Copy constructor (not implemented for this task)
   NetInTask(const NetInTask& rNetInTask);

     /// Assignment operator (not implemented for this task)
   NetInTask& operator=(const NetInTask& rhs);

};

#endif /* _INCLUDED_NETINTASKH ] */
