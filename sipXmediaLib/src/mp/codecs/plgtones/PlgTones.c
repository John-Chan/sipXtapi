//  
// Copyright (C) 2007 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: 

// APPLICATION INCLUDES
#include <mp/codecs/PlgDefsV1.h>
#include <mp/RtpHeader.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#ifdef __pingtel_on_posix__ // [
#include <sys/types.h>
#include <netinet/in.h>
#elif _VXWORKS // ][
#include <inetlib.h>
#elif defined WIN32 // _VXWORKS ][
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif // WIN32 ]

static const char codecMIMEsubtype[] = "telephone-events";

static const struct plgCodecInfoV1 codecTones = 
{
   sizeof(struct plgCodecInfoV1),  //cbSize
   codecMIMEsubtype,               //codecSDPType
   "tones",                        //codecName
   "telephone-events",             //codecVersion
   8000,                           //samplingRate
   0,                              //fmtAndBitsPerSample
   1,                              //numChannels
   0,                              //interleaveBlockSize
   6400,                           //bitRate
   128,                            //minPacketBits
   128,                            //avgPacketBits
   128,                            //maxPacketBits
   0,                              //numSamplesPerFrame
   0,                              //preCodecJitterBufferSize
   0,                              //codecSupportPLC
   TRUE                            //signalingCodec
};

struct AvtPacket
{
   uint8_t  key;
   uint8_t  dB;  ///< Bit allocation: 0-5 - Volume; 6 - Reserved; 7 - End bit.
   uint16_t duration;
};

struct tones_codec_data {
   int mHaveValidData;        ///< Does mLastRtpHeader and mLastPacketData
   ///< contain valid data?
   struct RtpHeader mLastRtpHeader;         ///< Copy of last received RTP packet header.
   struct AvtPacket mLastPacketData;        ///< Copy of last received RTP packet payload data.

   int mIsEventActive;        ///< TRUE, if some event is currently active.
   uint8_t mActiveEvent;             ///< The key ID of active event.
   RtpTimestamp mLastKeyUpTimetsamp; ///< The timestamp for last KEYUP event.
   RtpTimestamp mStartingTimestamp;  ///< The timestamp of active event.
   int32_t mLastEventDuration;       ///< Last reported event duration.

   int mPreparedDec;
   int mPreparedEnc;
};

static void resetEventState(struct tones_codec_data *mpTones)
{
   mpTones->mIsEventActive = FALSE;
   mpTones->mLastKeyUpTimetsamp = mpTones->mStartingTimestamp;
   mpTones->mStartingTimestamp = 0;
   mpTones->mLastEventDuration = 0;
}

#ifdef DEBUG_TONES // [
static void dumpRawAvtPacket(struct tones_codec_data *mpTones)
{
   uint8_t vpxcc;
   uint8_t mpt;
   RtpSeq seq;
   RtpTimestamp timestamp;
   RtpSRC ssrc;

   uint8_t  key;
   uint8_t  dB;
   UtlBoolean endBit;
   uint16_t duration;

   AvtPacket *pAvt;

   vpxcc = mpTones->mLastRtpHeader.vpxcc;
   mpt = mpTones->mLastRtpHeader.mpt;
   seq = ntohl(mpTones->mLastRtpHeader.seq);
   timestamp = ntohl(mpTones->mLastRtpHeader.timestamp);
   ssrc = ntohl(mpTones->mLastRtpHeader.ssrc);

   pAvt = (AvtPacket*)&mpTones->mLastPacketData;
   key = pAvt->key;
   dB  = pAvt->dB & 0x3F;
   endBit = pAvt->dB >> 7;
   duration = ntohs(pAvt->duration);

   printf("dumpTones(%p): Raw packet: "
      "seq:%6u ts:%10u key:%2u end:%1d dB:%3d duration:%5d\n",
      mpTones,
      seq, timestamp, key, endBit, dB, duration);
}
#endif // DEBUG_TONES ]

CODEC_API int PLG_ENUM_V1(tones)(const char** mimeSubtype, unsigned int* pModesCount, const char*** modes)
{
   if (mimeSubtype) {
      *mimeSubtype = codecMIMEsubtype;
   }
   if (pModesCount) {
      *pModesCount = 0;
   }
   if (modes) {
      *modes = NULL;
   }
   return RPLG_SUCCESS;
}

CODEC_API void *PLG_INIT_V1(tones)(const char* fmt, int bDecoder, struct plgCodecInfoV1* pCodecInfo)
{
   struct tones_codec_data *mpTones;
   if (pCodecInfo == NULL) {
      return NULL;
   }

   memcpy(pCodecInfo, &codecTones, sizeof(struct plgCodecInfoV1));
   mpTones = (struct tones_codec_data *)malloc(sizeof(struct tones_codec_data));
   if (!mpTones) {
      return NULL;
   }

   mpTones->mHaveValidData = FALSE;
   mpTones->mIsEventActive = FALSE;
   mpTones->mActiveEvent = 0;
   mpTones->mLastKeyUpTimetsamp = 0;
   mpTones->mStartingTimestamp = 0;
   mpTones->mLastEventDuration = 0;

   return mpTones;
}


CODEC_API int PLG_FREE_V1(tones)(void* handle, int isDecoder)
{
   struct tones_codec_data *mpTones = (struct tones_codec_data *)handle;

   if (NULL != handle)
   {
      free(handle);
   }
   return 0;
}

CODEC_API int PLG_DECODE_V1(tones)(void* handle, const void* pCodedData, 
                                   unsigned cbCodedPacketSize, void* pAudioBuffer, 
                                   unsigned cbBufferSize, unsigned *pcbDecodedSize, 
                                   const struct RtpHeader* pRtpHeader)
{
   struct tones_codec_data *mpTones = (struct tones_codec_data *)handle;
   assert(handle != NULL);

   // Just save passed packet header and data - they will be used
   // in getSignalingData() called right after decode().
   mpTones->mLastPacketData = *(const struct AvtPacket*) pCodedData;
   mpTones->mLastRtpHeader = *pRtpHeader;
   assert(mpTones->mHaveValidData == FALSE);
   mpTones->mHaveValidData = TRUE;

#ifdef DEBUG_TONES // [
   dumpRawAvtPacket(handle);
#endif // DEBUG_TONES ]

   return RPLG_SUCCESS;
}

CODEC_API int PLG_ENCODE_V1(tones)(void* handle, const void* pAudioBuffer, 
                                   unsigned cbAudioSamples, int* rSamplesConsumed, 
                                   void* pCodedData, unsigned cbMaxCodedData, 
                                   int* pcbCodedSize, unsigned* pbSendNow)
{
   int size = 0;   
   struct tones_codec_data *mpTones = (struct tones_codec_data *)handle;
   assert(handle != NULL);
   assert(!"Not yet implimented!!!");

   *rSamplesConsumed = 0;
   *pcbCodedSize = 0;
   *pbSendNow = FALSE;

   return RPLG_FAILED;
}

CODEC_API int PLG_SIGNALING_V1(tones)(void* handle, int dataId, uint32_t* outEvent, 
                                      uint32_t* outDuration, uint32_t* startStatus, uint32_t *stopStatus)
{
   RtpTimestamp ts;

   struct tones_codec_data *mpTones = (struct tones_codec_data *)handle;
   assert(handle != NULL);

   ts = ntohl(mpTones->mLastRtpHeader.timestamp);

   // == Stage 0 ==
   // Just return, if we have nothing to decode
   if (mpTones->mHaveValidData == FALSE)
   {
      return RPLG_NO_MORE_DATA;
   }

   // == Stage 1 ==
   // If this is a delayed packet from last event, we could drop it safely.
   // RFC4733 recommend to send end-of-event packet three times, so it's
   // normal to hit this check twice if no packets are lost.
   // Note: actually this check should use MpDspUtils::compareSerials(),
   //       but then we have to keep track of current stream timestamp.
   //       Else timestamp may wrap around between events, as much may
   //       pass between them. Later we could call decode() or getSignalingData()
   //       even if no packet is received to make this real.
   if (mpTones->mLastKeyUpTimetsamp == ts)
   {
      mpTones->mHaveValidData = FALSE;
      return RPLG_NO_MORE_DATA;
   }

   // == Stage 2 ==
   // Signal key up for previous tone if we lost its end packets.
   if (  mpTones->mIsEventActive            // (1) Previous tone still active
      && mpTones->mStartingTimestamp != ts  // (2) This is not an update packet for active event
      )
   {
      // Fill in returned data.
      *outEvent = mpTones->mActiveEvent;
      *startStatus = FALSE;
      *stopStatus = TRUE;
      *outDuration = mpTones->mLastEventDuration;

      // Reset decoder to no-active-event state. Saved RTP data and header are
      // kept as is to be used in next call to getSignalingData().
      resetEventState(mpTones);

      // Ok, we have something to signal.
      return RPLG_SUCCESS;
   }

   // == Stage 3 ==
   // Parse received RTP packet.

   // Update last received event duration.
   mpTones->mLastEventDuration = ntohs(mpTones->mLastPacketData.duration);

   // Check for start of event conditions
   if (  // start bit marked
      ((mpTones->mLastRtpHeader.mpt & RTP_M_MASK) && (ts != mpTones->mStartingTimestamp))
      || // key up interpreted as key down if no previous start event received 
      (mpTones->mIsEventActive == FALSE)
      )
   {
      mpTones->mIsEventActive = TRUE;
      mpTones->mActiveEvent = mpTones->mLastPacketData.key;
      mpTones->mStartingTimestamp = ts;

      *startStatus = TRUE;
   }
   else
   {
      *startStatus = FALSE;
   }

   // Check for end of event condition
   if ((0x80 & mpTones->mLastPacketData.dB) > 0)
   {
      // Reset decoder to no-active-event state.
      resetEventState(mpTones);

      *stopStatus = TRUE;
   }
   else
   {
      *stopStatus = FALSE;
   }

   // Signal that packet was parsed.
   mpTones->mHaveValidData = FALSE;

   // Fill in remaining data to be returned.
   *outEvent = mpTones->mLastPacketData.key;
   *outDuration = mpTones->mLastPacketData.duration;

   // Ok, we have something to signal.
   return RPLG_SUCCESS;

}

PLG_SINGLE_CODEC(tones);
