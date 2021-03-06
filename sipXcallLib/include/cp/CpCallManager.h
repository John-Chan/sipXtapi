//
// Copyright (C) 2005-2012 SIPez LLC. All rights reserved.
// 
// Copyright (C) 2004-2008 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Daniel Petrie dpetrie AT SIPez DOT com

#ifndef _CpCallManager_h_
#define _CpCallManager_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <os/OsServerTask.h>
#include <os/OsRWMutex.h>
#include "os/OsProtectEvent.h"
#include "os/OsQueuedEvent.h"

#include "ptapi/PtEvent.h"
#include "ptapi/PtDefs.h"
#include "net/SipMessage.h"
#include "net/SipContactDb.h"
#include "net/SipDialog.h"
#include "cp/Connection.h"
#include "tapi/sipXtapiInternal.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class CpCall;
class CpMediaInterface;
class SipSession;
class SipDialog;
class MpStreamPlayer;
class MpStreamPlaylistPlayer;
class OsEvent;

//! Abstract call manager
/*! There are three major components to the call management system:
 *\par
 * Call management methods
 *\par
 * Call model events
 *\par
 * Abstract media control interface.
 * \par
 * The call management methods provide the means to perform call
 * control operations and poll call state. The call model events
 * provide the means to listen for call model state changes. The
 * abstract media control interface provides the means to override
 * the media subsystem.
 * \par
 * Due to the transient nature of the objects in the call model,
 * handles or names are used to represent the actual objects.
 * Operations are performed on these objects via the Call Manager
 * by naming the object(s) when invoking a method.
 * /par
 * The INFINITY.0 APIs above use the same call model used in JTAPI
 *(as defined by the Enterprise Computer Telephony Forum). The
 * primary objects defined in this model are:
 *\par
 * call
 *\par
 * connection
 *\par
 * address
 *\par
 * terminal
 *\par
 * terminal connection.
 * \par
 * A call contains zero or more connections. A connection is
 * associated with an address (that is, a SIP URL). A terminal
 * connection is the relationship between a connection and a
 * terminal.
 * \par
 * Call Model state changes are notified through an event handler.
 *  The event handler is a sub-class of TaoAdaptor that implements
 * the handleMessage method.  This method must implement actions or
 * state caching of events of interest to the application.  The
 * events that are notified from the call manager subsystem are
 * enumerated in PtEvent.
 */
class CpCallManager : public OsServerTask
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    enum EventSubTypes
        {
        CP_UNSPECIFIED = SipMessage::NET_UNSPECIFIED,
        CP_SIP_MESSAGE = SipMessage::NET_SIP_MESSAGE,
        CP_CALL_EXITED,
        CP_DIAL_STRING,
        CP_FOCUS_CALL,
        CP_HOLD_CALL,
        CP_OFF_HOLD_CALL,
        CP_DEQUEUED_CALL,
        CP_MGCP_MESSAGE,
        CP_MGCP_CAPS_MESSAGE,
        CP_YIELD_FOCUS,  //10
        CP_GET_FOCUS,
        CP_CREATE_CALL,
        CP_GET_CALLS,
        CP_CONNECT,
        CP_SINGLE_CALL_TRANSFER,
        CP_BLIND_TRANSFER,
        CP_CONSULT_TRANSFER,
        CP_TRANSFER_CONNECTION,
        CP_TRANSFER_CONNECTION_STATUS,
        CP_TRANSFEREE_CONNECTION, //20
        CP_TRANSFEREE_CONNECTION_STATUS,
        CP_DROP,
        CP_DROP_CONNECTION,
        CP_FORCE_DROP_CONNECTION,
        CP_ANSWER_CONNECTION,
        CP_ACCEPT_CONNECTION,
        CP_REJECT_CONNECTION,
        CP_REDIRECT_CONNECTION,
        CP_GET_NUM_CONNECTIONS,
        CP_GET_CONNECTIONS, //30
        CP_GET_CALLED_ADDRESSES,
        CP_GET_CALLING_ADDRESSES,
        CP_START_TONE_TERM_CONNECTION,
        CP_STOP_TONE_TERM_CONNECTION,
        CP_PLAY_AUDIO_TERM_CONNECTION,
        CP_STOP_AUDIO_TERM_CONNECTION,
        CP_GET_NUM_TERM_CONNECTIONS,
        CP_GET_TERM_CONNECTIONS,
        CP_IS_LOCAL_TERM_CONNECTION,
        CP_HOLD_TERM_CONNECTION, //40
        CP_UNHOLD_TERM_CONNECTION,
        CP_UNHOLD_LOCAL_TERM_CONNECTION,
        CP_HOLD_LOCAL_TERM_CONNECTION,
        CP_OFFERING_EXPIRED,
        CP_RINGING_EXPIRED,
        CP_GET_CALLSTATE,
        CP_GET_CONNECTIONSTATE,
        CP_GET_TERMINALCONNECTIONSTATE,
        CP_GET_SESSION,
        CP_HOLD_ALL_TERM_CONNECTIONS,  //50
        CP_UNHOLD_ALL_TERM_CONNECTIONS,
        CP_CANCEL_TIMER,
        CP_GET_NEXT_CSEQ,
        CP_PLAY_BUFFER_TERM_CONNECTION,
        CP_CREATE_PLAYER,
        CP_DESTROY_PLAYER,
        CP_CREATE_PLAYLIST_PLAYER,
        CP_DESTROY_PLAYLIST_PLAYER,
        CP_CREATE_QUEUE_PLAYER,
        CP_DESTROY_QUEUE_PLAYER, //60
        CP_RENEGOTIATE_CODECS_CONNECTION,
        CP_RENEGOTIATE_CODECS_ALL_CONNECTIONS,
        CP_SET_CODEC_CPU_LIMIT,  
        CP_GET_CODEC_CPU_COST,
        CP_GET_CODEC_CPU_LIMIT,
        CP_SET_INBOUND_CODEC_CPU_LIMIT,
        CP_SET_OUTBOUND_LINE,
        CP_GET_LOCAL_CONTACTS,
        CP_OUTGOING_INFO,
        CP_GET_MEDIA_CONNECTION_ID, //70
        CP_ENABLE_STUN,
        CP_ENABLE_TURN,
        CP_GET_CAN_ADD_PARTY, 
        CP_SPLIT_CONNECTION,
        CP_JOIN_CONNECTION,
        CP_CONSULT_TRANSFER_ADDRESS,
        CP_START_TONE_CONNECTION,
        CP_STOP_TONE_CONNECTION,
        CP_PLAY_AUDIO_CONNECTION,
        CP_STOP_AUDIO_CONNECTION, //80
        CP_TRANSFER_OTHER_PARTY_HOLD,
        CP_TRANSFER_OTHER_PARTY_JOIN,
        CP_TRANSFER_OTHER_PARTY_UNHOLD,
        CP_GET_MEDIA_ENERGY_LEVELS,
        CP_GET_CALL_MEDIA_ENERGY_LEVELS,
        CP_GET_MEDIA_RTP_SOURCE_IDS,
        CP_RECORD_AUDIO_CONNECTION_START,
        CP_RECORD_AUDIO_CONNECTION_STOP,
        CP_RECORD_BUFFER_AUDIO_CONNECTION_START,
        CP_RECORD_BUFFER_AUDIO_CONNECTION_STOP, //90
        CP_LIMIT_CODEC_PREFERENCES,
        CP_SILENT_REMOTE_HOLD, 
        CP_GET_USERAGENT,
        CP_FLOWGRAPH_MESSAGE,
        CP_SET_MIC_GAIN,
        CP_SET_MEDIA_PASS_THROUGH,
        CP_CREATE_MEDIA_CONNECTION,
        CP_SET_RTP_DESTINATION,
        CP_START_RTP_SEND,
        CP_STOP_RTP_SEND,  //100
        CP_LIMIT_CODECS,
        CP_SET_OUTPUT_MIX_WEIGHT
    };
#endif // DOXYGEN_SHOULD_SKIP_THIS

    enum CallTypes
    {
        SIP_CALL = 0,
        MGCP_CALL
    };

    enum CallHoldType
    {
        NEAR_END_HOLD = 0,
        FAR_END_HOLD
    };

    enum CpStatus
    {
        CP_SUCCESS = 0,
        CP_FAILED,
        CP_INVALID_IP_ADDRESS,
        CP_INVALID_SIP_DIRECTORY_SERVER,
        CP_INVALID_SIP_URL
    };

/* ============================ CREATORS ================================== */

    //! Default constructor
    CpCallManager(const char* taskName,
                 const char* callIdPrefix,
                 int rtpPortStart = 8766,
                 int rtpPortEnd = -1,
                 const char* localAddress = NULL,
                 const char* publicAddress = NULL,
                 int   internalSamplerate = 8000);

    //! Destructor
    virtual
    ~CpCallManager();

/* ============================ MANIPULATORS ============================== */

    static void getEventSubTypeString(EventSubTypes type,
                                      UtlString& typeString);

    //! Set the default address for the local connection.
    /*! This is used to set the calling ID information when
     * making an outbound call.
     */
    virtual void setOutboundLine(const char* lineUrl) = 0;

    /** @name For internal use only
     * These should be private methods, but due to the structure
     * of how they are used they must be public.
     */
    //@{

    //! For internal use only
    virtual UtlBoolean handleMessage(OsMsg& eventMessage) = 0;

    //! For internal use only
    virtual void getNewCallId(UtlString* callId);

    //! Generate a new Call-Id with the specified prefix.
    static void getNewCallId(const char* callIdPrefix, UtlString* callId);

    //! For internal use only
    void getNewSessionId(UtlString* sessionId);

    //! For internal use only
    int getNewMetaEventId();

    //@}

    /** @name Call Operations
     */
    //@{

    //! Creates a new call with an implicit local connection.
    virtual void createCall(UtlString* callId,
                            int metaEventId = 0,
                            int metaEventType = PtEvent::META_EVENT_NONE,
                            int numMetaEventCalls = 0,
                            const char* callIds[] = NULL,
                            UtlBoolean assumeFocusIfNoInfocusCall = TRUE) = 0;

    //! Gets the list of names or identifiers for all of the
    //! existing calls.
    /*! Note: Do not assume that the callIds returned
     * are the same as those used in the signalling world
     * (e.g. SIP call-id for a connection may not be the
     * same as the callId used to represent the call or
     * connections)
     */
    virtual OsStatus getCalls(int maxCalls, int& numCalls,
                               UtlString callIds[]) = 0;

    //! Initiates a new outbound connection to the specified address.
    /*! This may be invoked multiple times on a call to create
     * bridged conference.
     */
    virtual PtStatus connect(const char* callId,
                             const char* toAddress,
                             const char* fromAddress = NULL,
                             const char* desiredConnectionCallId = NULL,
                             SIPX_CONTACT_ID contactId = 0,
                             const void* pDisplay = NULL,
                             const void* pSecurity = NULL,
                             const char* locationHeader = NULL,
                             const int bandWidth=AUDIO_CODEC_BW_DEFAULT,
                             SIPX_TRANSPORT_DATA* pTransportData = NULL,
                             const RTP_TRANSPORT rtpTransportOptions = RTP_TRANSPORT_UDP) = 0;

    //! Create a new call and associate it with an existing call.
    /*! This is usually done to create the consultative call as a
     * precursor to performing a transfer.
     */
    virtual PtStatus consult(const char* idleTargetCallId,
                             const char* activeOriginalCallId,
                             const char* originalCallControllerAddress,
                             const char* originalCallControllerTerminalId,
                             const char* consultAddressUrl,
                             UtlString& targetCallControllerAddress,
                             UtlString& targetCallConsultAddress) = 0;

    //! Blind transfer
    virtual PtStatus transfer_blind(const char* callId,
                              const char* transferToUrl,
                              UtlString* targetCallId,
                              UtlString* targetConnectionAddress = NULL) = 0;

    //! Consultative transfer
    /*! This transfer method is used to perform the transfer after
     * completing a consultative call. The consultative call must
     * be created using the \e consult() method
     * The couple \a targetCallId & \a targetConnectionAddress define
     * the transfer target connection in the resulting new transfer
     * target call
     */
    virtual PtStatus transfer(const char* targetCallId,
                              const char* originalCallId) = 0;

    //! Drop this call and disconnect all connections associated with it.
    virtual void drop(const char* callId) = 0;

    //! Direct the media subsystem to begin playing a DTMF or progress tone.
    virtual void toneStart(const char* callId,
                           int toneId,
                           UtlBoolean local,
                           UtlBoolean remote) = 0;

    //! Direct the media subsystem to stop playing a DTMF or progress tone.
    virtual void toneStop(const char* callId) = 0;

    //! Direct the media subsystem to begin playing a DTMF or progress tone.
    virtual void toneChannelStart(const char* callId,
                                  const char* szRemoteAddress,
                                  int toneId,
                                  UtlBoolean local,
                                  UtlBoolean remote) = 0;

    //! Direct the media subsystem to stop playing a DTMF or progress tone.
    virtual void toneChannelStop(const char* callId,
                                 const char* szRemoteAddress) = 0;

    //! Deprecated, use the player controls
    /*! Direct the media subsystem to play audio from an external source
     * accessed via a URL.
     */
    virtual void audioPlay(const char* callId,
                           const char* audioUrl,
                           UtlBoolean repeat,
                           UtlBoolean local,
                           UtlBoolean remote,
                           UtlBoolean mixWithMic = false, 
                           int downScaling = 100) = 0;

    //! Deprecated, use the player controls
    /*! Direct the media subsystem to stop playing audio
     */
    virtual void audioChannelStop(const char* callId,
                                  const char* szRemoteAddress) = 0;

    //! Deprecated, use the player controls
    /*! Direct the media subsystem to play audio from an external source
     * accessed via a URL.
     */
    virtual void audioChannelPlay(const char* callId,
                                  const char* szRemoteAddress,
                                  const char* audioUrl,
                                  UtlBoolean repeat,
                                  UtlBoolean local,
                                  UtlBoolean remote,
                                  UtlBoolean mixWithMic = false, 
                                  int downScaling = 100) = 0;

    //! Deprecated, use the player controls
    /*! Direct the media subsystem to stop playing audio.
     */
    virtual void audioStop(const char* callId) = 0;

    //! Deprecated, use the player controls
    /*! Direct the media subsystem to play audio from a data buffer.
     */
    virtual void bufferPlay(const char* callId,
                            const void* audiobuf,
                            int bufSize,
                            int type,
                            UtlBoolean repeat,
                            UtlBoolean local,
                            UtlBoolean remote) = 0;

    //! Create a MpStreamPlaylistPlayer media player associated with
    /*! the specified call. The media player can subsequently be used
     * to play media such as streamed audio to the connections
     * (local and remote) in this call. The streamed audio source can
     * be a set on one or more audio URLs that correspond to audio
     * snippets that the player will stream in a concatenated set.
     */
    virtual void createPlayer(const char* callid,
                              MpStreamPlaylistPlayer** ppPlayer) = 0 ;

    //! Create a media player associated with the specified call.
    /*! The media player can subsequently be used to play media
     * such as streamed audio to the connections (local and remote)
     * in this call. The streamed audio source can be a single
     * audio URL or a set of URLs that correspond to audio
     * snippets that the player will stream in a concatenated
     * set. Currently two types of media players are supported:
     * \par
     * 1) A simple player that buffers some of the media and then
     * starts playing a single media source (URL or stream).
     * \par
     * 2) A queued player that supports two buffered play lists
     * where there is one active play list that can be played,
     * paused or stopped. The active buffer list can be changed
     * on the fly and sources can be added to either buffer list.
     * The effect of this player is similar to graphical double
     * buffering where one buffer can be filling while the other
     * is playing.
     */
    virtual void createPlayer(int type,
                              const char* callid,
                              const char* szStream,
                              int flags,
                              MpStreamPlayer** ppPlayer) = 0 ;

    //! Destroy the media player associated with a call.
    virtual void destroyPlayer(const char* callid,
                               MpStreamPlaylistPlayer* pPlayer) = 0 ;


    //! Destroy the media player associated with a call.
    virtual void destroyPlayer(int type,
                               const char* callid,
                               MpStreamPlayer* pPlayer) = 0 ;


    //!Sets the CPU codec limit for a call.
    /*! Each connection within the call
     * may only use codecs whose CPU requirements are less than or
     * equal to the specified limit.
     */
    virtual OsStatus setCodecCPULimitCall(const char* callId,
                                          int limit,
                                          UtlBoolean bRenegotiate) = 0 ;

    //! Set the call codec CPU limit for inbound connections in a call.
    virtual OsStatus setInboundCodecCPULimit(int limit) = 0 ;

    /// Sets the Mic gain.
    virtual OsStatus setMicGain(const char* callId, float gain) = 0;

    //@{

    //! Accept the incoming connection
    /*! Progress the connection from the OFFERING state to the
     * RINGING state. This causes a SIP 180 Ringing provisional
     * response to be sent.
     */
    virtual void acceptConnection(const char* callId,
                                  const char* address,
                                  SIPX_CONTACT_ID contactId = 0,
                                  const void* hWnd = NULL,
                                  const void* security = NULL,
                                  const char* locationHeader = NULL,
                                  const int bandWidth=AUDIO_CODEC_BW_DEFAULT,
                                  UtlBoolean sendEarlyMedia = FALSE) = 0;

    virtual void setOutboundLineForCall(const char* callId, 
                                        const char* address, 
                                        SIPX_CONTACT_TYPE eType = CONTACT_AUTO) = 0;


    //! Reject the incoming connection
    /*! Progress the connection from the OFFERING state to
     * the FAILED state with the cause of busy. With SIP this
     * causes a  486 Busy Here response to be sent.
     */
    virtual void rejectConnection(const char* callId,
                                  const char* address, 
                                  int errorCode = 0, 
                                  const char* errorText =  "") = 0;

    //! Redirect the incoming connection
    /*! Progress the connection from the OFFERING state to
     * the FAILED state. This causes a SIP 302 Moved
     * Temporarily response to be sent with the specified
     * contact URI.
     */
    virtual PtStatus redirectConnection(const char* callId,
                                        const char* address,
                                        const char* forwardAddressUrl) = 0;

    //! Drop the specifed connection
    /*! The approriate disconnect signal is sent
     * (e.g. with SIP BYE or CANCEL).  The connection state
     * progresses to disconnected and the connection is removed.
     */
    virtual void dropConnection(const char* callId,
                                const char* address) = 0;

    //! Query the number of connections in the specified call.
    virtual void getNumConnections(const char* callId,
                                    int& numConnections) = 0;

    //! Query the list of addresses or handles for the connections
    //! in the specified call.
    virtual OsStatus getConnections(const char* callId,
                                     int maxConnections,
                                     int& numConnections,
                                     UtlString addresses[]) = 0;

    //! Query the list of addresses or handles for the connections
    //! in the specified call that were set up as outbound connections.
    virtual OsStatus getCalledAddresses(const char* callId,
                                        int maxConnections,
                                        int& numConnections,
                                        UtlString addresses[]) = 0;

    //! Query the list of addresses or handles for the connections in
    //! the specified call that were set up as inbound connections.
    virtual OsStatus getCallingAddresses(const char* callId,
                                         int maxConnections,
                                         int& numConnections,
                                         UtlString addresses[]) = 0;

    //@}

    /** @name Call & Terminal Connection Operations
     * This set of methods perform operations on calls and
     * terminal connections.
     */
    //@{

    //! Answer the incoming terminal connection
    /*! Progress the connection from the OFFERING or RINGING state
     * to the ESTABLISHED state and also creating the terminal
     * connection (with SIP a 200 OK response is sent).
     */
    virtual void answerTerminalConnection(const char* callId,
                                          const char* address,
                                          const char* terminalId,
                                          const void* pDisplay = NULL,
                                          const void* pSecurity = NULL) = 0;

    //! Put the specified terminal connection on hold
    /*! Change the terminal connection state from TALKING to HELD.
      * (With SIP a re-INVITE message is sent with SDP indicating
      * no media should be sent.)
      */
    virtual void holdTerminalConnection(const char* callId,
                                        const char* address,
                                        const char* terminalId) = 0;

    //! Convenience method to put all of the terminal connections in
    //! the specified call on hold.
    virtual void holdAllTerminalConnections(const char* callId) = 0;

    //! Convenience method to put the local terminal connection on hold.
    virtual void holdLocalTerminalConnection(const char* callId) = 0;

    //! Take the specified terminal connection off hold,
    /*! Change the terminal connection state from HELD to TALKING.
     * (With SIP a re-INVITE message is sent with SDP indicating
     * media should be sent.)
     */
    virtual void unholdLocalTerminalConnection(const char* callId) = 0;

    //! Convenience method to take all of the terminal connections in
    //! the specified call off hold.
    virtual void unholdAllTerminalConnections(const char* callId) = 0;

    //! Convenience method to take the local terminal connection off hold.
    virtual void unholdTerminalConnection(const char* callId,
                                          const char* addresss,
                                          const char* terminalId) = 0;

    //! Further limit the set of codecs to use for the call to the given set of codecs
    //! The codecs named, must be a subset of those enabled for the call.
    virtual void limitCodecs(const char* callId,
                             const char* remoteAddr,
                             const char* codecNames) = 0;
    
    //! Rebuild codec factory on the fly with new audio codec requirements
    //! and one specific video codec
    virtual void limitCodecPreferences(const char* callId,
                                       const char* remoteAddr,
                                       const int audioBandwidth,
                                       const int videoBandwidth,
                                       const char* szVideoCodecName) = 0;

    //! Renegotiate the codecs to be use for the sepcified terminal connection
    /*! This is typically performed after a capabilities change for the
     * terminal connection (for example, addition or removal of a codec type).
     * (Sends a SIP re-INVITE.)
     */
    virtual void renegotiateCodecsTerminalConnection(const char* callId,
                                                     const char* addresss,
                                                     const char* terminalId) = 0;

    //! Convenience method to renegotiate the codecs for all of the terminal
    //! connections in the specified call.
    virtual void renegotiateCodecsAllTerminalConnections(const char* callId) = 0 ;

    //! Query the number of terminal connections in the specified call.
        virtual void getNumTerminalConnections(const char* callId,
                                           const char* address,
                                           int& numTerminalConnections) = 0;

    //! Get the list of terminal connection identifiers for the specified call.
        virtual OsStatus getTerminalConnections(const char* callId,
                                            const char* address,
                                            int maxTerminalConnections,
                                            int& numTerminalConnections,
                                            UtlString terminalNames[]) = 0;

    //! Query whether the specified terminal connection is a local or remote connection.
    virtual UtlBoolean isTerminalConnectionLocal(const char* callId,
                                                const char* address,
                                                const char* terminalId) = 0;
    virtual void doGetFocus(CpCall* call) = 0;

    //! Get the SIP session information for the specified terminal connection.
    virtual OsStatus getSession(const char* callId,
                                const char* address,
                                SipSession& session) = 0;

    //! Get the SIP dialog information for the specified terminal connection.
    virtual OsStatus getSipDialog(const char* callId,
                                  const char* address,
                                  SipDialog& dialog) = 0;
    //@}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /** @name Stimulus based operations DEPRECATED DO NOT USE
     */
    //@{
    //! Deprecated, use holdAllTerminalConnections
    virtual void unhold(const char* callId) = 0;

    //! Deprecated, use connect
    virtual void dialString(const char* url) = 0;
    //@}
#endif // DOXYGEN_SHOULD_SKIP_THIS

        //! do-not-disturb flag
        virtual void setDoNotDisturb(int flag);

        //! msg waiting flag
        virtual void setMessageWaiting(int flag);

        //! offered time-out for all incoming calls
    /*! If a call is not accepted within this timeout period
     * it is automatically rejected.
     */
        virtual void setOfferedTimeout(int millisec);

        virtual UtlBoolean disconnectConnection(const char* callId,
                                           const char* addressUrl) = 0;

    //! Deprecated
    virtual void setTransferType(int type) = 0;

    virtual void enableIce(UtlBoolean bEnable) ;
    virtual void getRemoteUserAgent(const char* callId, 
                                    const char* remoteAddress,
                                    UtlString& userAgent) = 0;

    /**
     * Set the target sip url for voice quality reports
     */
    virtual void setVoiceQualityReportTarget(const char* szTargetSipUrl) ;


/* ============================ ACCESSORS ================================= */

   /**
    * Gets the number of lines made available by line manager.
    */
   virtual int getNumLines() = 0;

  /**
   * maxAddressesRequested is the number of addresses requested if available
   * numAddressesAvailable is the actual number of addresses available.
   * "addresses" is a pre-allocated array of size maxAddressesRequested
   */
   virtual OsStatus getOutboundAddresses(int maxAddressesRequested,
                                       int& numAddressesAvailable, UtlString** addresses) = 0;

    //! Get the state of the identified call.
        virtual UtlBoolean getCallState(const char* callId,
                                   int& state) = 0;

    //! Get the connection state for the specified connection.
    /*! Note: one should generally avoid polling of the state as
     * many race conditions occur.  The best way to get the state
     * is to create a listener that recieves state change notification
     * events.
     */
        virtual UtlBoolean getConnectionState(const char* callId,
                                                                                const char* remoteAddress,
                                                                                int& state) = 0;

    //! Get the terminal connection state for the specified terminal connection.
    /*! Note: one should generally avoid polling of the state as
     * many race conditions occur.  The best way to get the state
     * is to create a listener that recieves state change notification
     * events.
     */
        virtual UtlBoolean getTermConnectionState(const char* callId,
                                                                                        const char* address,
                                                                                        const char* terminal,
                                                                                        int& state) = 0;

    virtual PtStatus validateAddress(UtlString& address) = 0;

    //! Deprecated, use getSession
        virtual OsStatus getFromField(const char* callId,
                                  const char* remoteAddress,
                                  UtlString& fromField) = 0;

    //! Deprecated, use getSession
        virtual OsStatus getToField(const char* callId,
                                const char* remoteAddress,
                                UtlString& toField) = 0;

    //! Gets the CPU cost for an individual connection within the specified
    //! call.
    /*! This cost represents the current CPU cost for codec processing
     * for the connection. However, the actual CPU usage may be less depending
     * on whether the connection is on hold, the other party is silent, etc.
     */
    virtual OsStatus getCodecCPUCostCall(const char* callId,
                                         int& cost) = 0;

    //! Gets the CPU cost for an individual connection within the specified
    //! call.
    /*! This cost represents the maximum expected CPU cost for codec processing
     * for the connection. However, the actual CPU usage may be less depending
     * on whether the connection is on hold, the other party is silent, etc.
     */
    virtual OsStatus getCodecCPULimitCall(const char* callId,
                                          int& cost) = 0;

    virtual UtlBoolean isIceEnabled() const ;

    /**
     * Get the target sip url for voice quality reports
     */
    virtual UtlBoolean getVoiceQualityReportTarget(UtlString& reportSipUrl) ;

    /**
     * Get the configured local address
     */
    virtual void getLocalAddress(UtlString& address) ;

/* ============================ INQUIRY =================================== */

    virtual void onCallDestroy(CpCall* pCall) = 0;
    
   virtual void yieldFocus(CpCall* call) = 0;
    
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

    /*! Note: you better put a lock with the mCallListMutex around what ever
     * you do with call as this method only locks to retrieve.  There is
     * nothing that prevents the call from being deleted out from under you.
     */
    virtual CpCall* findCall(const char* callId);

    int aquireCallIndex();
    void releaseCallIndex(int callIndex);

    virtual void pushCall(CpCall* call);
    virtual void appendCall(CpCall* call);

    OsMutex mManagerMutex;
    OsRWMutex mCallListMutex;
    // Mutex to protect mCallNum.
    static OsMutex mCallNumMutex;
    UtlHashBag mCallIndices;
    UtlString mLocalAddress;
    UtlString mPublicAddress;
    int mRtpPortStart;
    int mRtpPortEnd;

    int mLineAvailableBehavior;
    UtlString mForwardUnconditional;

    int mLineBusyBehavior;
    UtlString mSipForwardOnBusy;

    int mNoAnswerTimeout;
    UtlString mForwardOnNoAnswer;

    int mDoNotDisturbFlag;
    int mMsgWaitingFlag;
    int mOfferedTimeOut;
    int mInviteExpireSeconds;  // The PHONESET_CP_RINGING_EXPIRE_SECONDS parameter,
                              // it is used to set the ringing expired timer if there
                              // is no Expires header field from an incoming INVITE
    int mDefaultSampleRate; ///< for flowgraph creation

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
    //! Maximum number of request messages
    static const int CALLMANAGER_MAX_REQUEST_MSGS;

    //! Copy constructor
    CpCallManager(const CpCallManager& rCpCallManager);

    //! Assignment operator
    CpCallManager& operator=(const CpCallManager& rhs);

    UtlString mCallIdPrefix;    
    UtlDList mCallList;
    int mLastMetaEventId;
    UtlBoolean mbEnableICE ;
    UtlString mVoiceQualityReportTarget ;

    // Every CallManager shares the same call counter for generating Call-IDs.
    static int64_t mCallNum;
 
#endif // DOXYGEN_SHOULD_SKIP_THIS

};

/* ============================ INLINE METHODS ============================ */

#endif  // _CpCallManager_h_
