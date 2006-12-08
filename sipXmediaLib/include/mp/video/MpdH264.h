//  
// Copyright (C) 2006 SIPfoundry Inc. 
// Licensed by SIPfoundry under the LGPL license. 
//  
// Copyright (C) 2006 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//  
// $$ 
////////////////////////////////////////////////////////////////////////////// 


#ifndef _MpdH264_h_
#define _MpdH264_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include "mp/MpVideoBuf.h"
#include "mp/MpRtpBuf.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class MpBufPool;
class MpCodecInfo;
struct AVCodecContext;
struct AVCodec;
struct AVFrame;

/// H.264 video stream decoder and RTP depacketizer.
/**
*  This class is aimed to restore H.264 frames from RTP packets (RTP
*  depacketization) and then decode them using FFMpeg library. So H.264 decoder
*  features are the same as FFMpeg H.264 decoder have. RTP depacketization
*  is compliant with RTP 3984 (RTP Payload Format for H.264 Video) and
*  support Single NAL Unit Mode and Non-Interleaved Mode. Interleaved Mode
*  is not supported - seems like it does not make big sense for video telephony.
*/
class MpdH264
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// Constructor
   MpdH264( int payloadType ///< (in) RTP payload type associated with
                            ///<  this decoder
          , MpBufPool *pVideoBufPool
          );

     /// Destructor
   virtual
   ~MpdH264();

     /// Initializes a codec data structure for use as a decoder
   virtual OsStatus initDecode();
     /**<
     *  @returns <b>OS_SUCCESS</b> - Success
     *  @returns <b>OS_NO_MEMORY</b> - Memory allocation failure
     */

     /// Frees all memory allocated to the decoder by <i>initDecode</i>
   virtual OsStatus freeDecode();
     /**<
     *  @returns <b>OS_SUCCESS</b> - Success
     *  @returns <b>OS_DELETED</b> - Object has already been deleted
     */

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

     /// Decode incoming RTP packet
   virtual MpVideoBufPtr decode(const MpRtpBufPtr &pPacket ///< (in) Pointer to a media buffer
                               , bool &packetConsumed ///< (out) Is packet consumed by decoder
                                                      ///< or should be passed to next call to decoder.
                               , bool forceFlag=false
                               );
     /**<
     *  @return Number of decoded samples.
     */

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

   MpBufPool      *mpVideoBufPool;  ///< Buffer pool for decoded frames.
   AVCodec        *mpCodec;
   AVCodecContext *mpCodecContext;
   AVFrame        *mpPicture;

   enum {
      ENCODED_BUFFER_SIZE=(352*288*3) ///< Initial size of mpFrameBuf.
   };

   enum {
      PACKET_DROP,
      PACKET_COLLECT,
      PACKET_REPEAT
   };

   // Depacketizer variables.
   UCHAR   *mpFrameBuf;
   UCHAR   *mpFrameBufEnd;
   UCHAR   *mpDecFrameBuf;
   int      mDecFrameBufSize;
   int      mNALuOctet;
   int      mFrameFBit;
   RtpSeq   mPreviousSeqNum;        ///< Sequence number of last received packet
   RtpTimestamp mPreviousTimeStamp; ///< Timestamp of last received packet
   bool     mSessionInitialized;    ///< Does we received first RTP packet?

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:


};

/* ============================ INLINE METHODS ============================ */

#endif  // _MpdH264_h_
