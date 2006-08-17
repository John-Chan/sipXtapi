//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _MprEncode_h_
#define _MprEncode_h_

#include "mp/MpMisc.h"

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "mp/MpAudioResource.h"
#include "net/SdpCodec.h"
#include "mp/MpFlowGraphMsg.h"
#include "mp/MprToNet.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class MpEncoderBase;

/**
*  @brief The "Encode" media processing resource.
*/
class MprEncode : public MpAudioResource
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

   MprEncode(const UtlString& rName, int samplesPerFrame, int samplesPerSec);
     //:Constructor

   virtual
   ~MprEncode();
     //:Destructor

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

   OsStatus selectCodecs(SdpCodec* pPrimaryCodec,
                          SdpCodec* pDtmfCodec,
                          SdpCodec* pSecondaryCodec);

   OsStatus deselectCodecs(void);

   OsStatus setNetFrameSize(int samples);

   void setMyToNet(MprToNet* myToNet);

   OsStatus startTone(int toneId);

   OsStatus stopTone(void);

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
   enum AddlMsgTypes
   {
      SELECT_CODECS = MpFlowGraphMsg::RESOURCE_SPECIFIC_START,
      DESELECT_CODECS,
      START_TONE,
      STOP_TONE
   };

   enum {TONE_STOP_PACKETS = 3}; // MUST BE > 0
   enum {HANGOVER_PACKETS = 25}; // At 20 ms each, 500 ms.

   static const int RTP_KEEP_ALIVE_FRAME_INTERVAL;

   MpEncoderBase* mpPrimaryCodec;
   unsigned char* mpPacket1Buffer;  ///< packet buffer for primary RTP stream
   unsigned char* mpPacket1Payload;
   int   mPacket1PayloadBytes;
   int   mPacket1PayloadUsed;
   unsigned int mStartTimestamp1;
   UtlBoolean mActiveAudio1;
   UtlBoolean mMarkNext1;
   int   mConsecutiveInactive1;
   int   mConsecutiveActive1;
   int   mConsecutiveUnsentFrames1;
   UtlBoolean mDoesVad1;

   MpEncoderBase* mpDtmfCodec;
   unsigned char* mpPacket2Buffer;  ///< packet buffer for DTMF event RTP stream
   unsigned char* mpPacket2Payload;
   int   mPacket2PayloadBytes;      ///< 4
   int   mPacket2PayloadUsed;       ///< not really needed
   unsigned int   mStartTimestamp2; ///< sample time when tone starts
   unsigned int   mLastDtmfSendTimestamp;
   int   mDtmfSampleInterval;       ///< # samples between AVT packets
   int   mCurrentTone;  ///< AVT event code for current tone
   int   mNumToneStops; ///< set to # of end packets to send when tone stops
   int   mTotalTime;    ///< # samples tone was active, set when tone stops
   int   mNewTone;      ///< set when tone starts

   MpEncoderBase* mpSecondaryCodec;
   unsigned char* mpPacket3Buffer;  ///< packet buffer for secondary RTP stream
   unsigned char* mpPacket3Payload;
   int   mPacket3PayloadBytes;
   int   mPacket3PayloadUsed;
   unsigned int   mStartTimestamp3;
   UtlBoolean mActiveAudio3;
   UtlBoolean mMarkNext3;
   int   mConsecutiveInactive3;
   int   mConsecutiveActive3;
   int   mConsecutiveUnsentFrames3;
   UtlBoolean mDoesVad3;

   unsigned int   mLastTimestamp;

   MprToNet* mpToNet;

   virtual UtlBoolean doProcessFrame(MpBufPtr inBufs[],
                                    MpBufPtr outBufs[],
                                    int inBufsSize,
                                    int outBufsSize,
                                    UtlBoolean isEnabled,
                                    int samplesPerFrame=80,
                                    int samplesPerSecond=8000);

     /// Handle messages for this resource.
   virtual UtlBoolean handleMessage(MpFlowGraphMsg& rMsg);

     /// @brief Get maximum payload size, estimated from
     /// MpEncoderBase::getMaxPacketBits().
   int payloadByteLength(MpEncoderBase& rEncoder);

   OsStatus allocPacketBuffer(MpEncoderBase& rEncoder,
                              unsigned char*& rpPacketBuffer,
                              unsigned char*& rpPacketPayload,
                              int& rPacketPayloadBytes,
                              int& rPacketPayloadUsed);

   void handleSelectCodecs(MpFlowGraphMsg& rMsg);

   void handleDeselectCodecs(void);

   int lookupTone(int toneId);

   void handleStartTone(int toneId);

   void handleStopTone(void);

   void doPrimaryCodec(MpAudioBufPtr in, unsigned int startTs);

   void doDtmfCodec(unsigned int startTs, int sPFrame, int sPSec);

   void doSecondaryCodec(MpAudioBufPtr in, unsigned int startTs);

     /// Copy constructor (not implemented for this class)
   MprEncode(const MprEncode& rMprEncode);

     /// Assignment operator (not implemented for this class)
   MprEncode& operator=(const MprEncode& rhs);

};

/* ============================ INLINE METHODS ============================ */

#endif  // _MprEncode_h_
