//  
// Copyright (C) 2006-2007 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2004-2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _MpRtpOutputConnection_h_
#define _MpRtpOutputConnection_h_

#include "rtcp/RtcpConfig.h"

// FORWARD DECLARATIONS
class MprDejitter;
class MprToNet;
class MprFromNet;
class OsSocket;
class SdpCodec;
#ifdef INCLUDE_RTCP /* [ */
struct IRTCPSession;
struct IRTCPConnection;
#endif /* INCLUDE_RTCP ] */

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include "os/OsMutex.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
typedef int MpConnectionID;

/**
*  @brief Connection container for the inbound and outbound network paths to a
*  single remote party.
*/
class MpRtpOutputConnection
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

   enum {
      NUM_PAYLOAD_TYPES = (2<<7),
      MAX_ACTIVE_PAYLOAD_TYPES = 10
   };

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// Constructor
   MpRtpOutputConnection(MpConnectionID myID, IRTCPSession *piRTCPSession);

     /// Destructor
   virtual
   ~MpRtpOutputConnection();

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

     /// Disables output path of the connection.
   // virtual OsStatus disable(); 
     /**<
     *  Resources on the path(s) will also be disabled by these calls.
     *  If the flow graph is not "started", this call takes effect
     *  immediately.  Otherwise, the call takes effect at the start of the
     *  next frame processing interval.
     *
     *  @returns <b>OS_SUCCESS</b> - for now, these methods always return success
     */

     /// Enables output path of the connection.
   virtual OsStatus enable();
     /**<
     *  Resources on the path(s) will also be enabled by these calls.
     *  Resources may allocate needed data (e.g. output path reframe buffer)
     *   during this operation.
     *  If the flow graph is not "started", this call takes effect
     *  immediately.  Otherwise, the call takes effect at the start of the
     *  next frame processing interval.
     *
     *  @returns <b>OS_SUCCESS</b> - for now, these methods always return success
     */

     /// Starts sending RTP and RTCP packets.
   void prepareStartSendRtp(OsSocket& rRtpSocket, OsSocket& rRtcpSocket);
     /**<
     *  @note: Someday may be made protected, if MpVideoCallFlowGraph will not
     *         need access to it.
     */

     /// Stops sending RTP and RTCP packets.
   void prepareStopSendRtp();
     /**<
     *  @note: Someday may be made protected, if MpVideoCallFlowGraph will not
     *         need access to it.
     */

#ifdef INCLUDE_RTCP /* [ */
     /// A new SSRC has been generated for the Session
   void reassignSSRC(int iSSRC);
#endif /* INCLUDE_RTCP ] */

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

     /// Return pointer to RTP writer component of connection.
   MprToNet *getRtpWriter() const {return mpToNet;}
     /**<
     *  This component should be used to send RTP packets to remote party.
     */

#ifdef INCLUDE_RTCP /* [ */
     /// Retrieve the RTCP Connection interface associated with this MpRtpOutputConnection
   IRTCPConnection *getRTCPConnection(void);
#endif /* INCLUDE_RTCP ] */

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

   MprToNet*          mpToNet;         ///< Outbound component: ToNet
   MpConnectionID     mMyID;           ///< ID within parent flowgraph
   UtlBoolean         mOutEnabled;     ///< Current state of outbound components
   UtlBoolean         mOutRtpStarted;  ///< Are we currently sending RTP stream?
   OsMutex            mLock;

#ifdef INCLUDE_RTCP /* [ */
   IRTCPSession    *mpiRTCPSession;    ///< RTCP Session Interface pointer
   IRTCPConnection *mpiRTCPConnection; ///< RTCP Connection Interface pointer

#endif /* INCLUDE_RTCP ] */

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

     /// Default constructor
   MpRtpOutputConnection();

     /// Copy constructor (not implemented for this type)
   MpRtpOutputConnection(const MpRtpOutputConnection& rMpRtpOutputConnection);

     /// Assignment operator (not implemented for this type)
   MpRtpOutputConnection& operator=(const MpRtpOutputConnection& rhs);
};

/* ============================ INLINE METHODS ============================ */

#endif  // _MpRtpOutputConnection_h_