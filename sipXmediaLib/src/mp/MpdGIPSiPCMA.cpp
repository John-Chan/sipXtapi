//  
// Copyright (C) 2006 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_GIPS /* [ */

#include "assert.h"
// APPLICATION INCLUDES
#include "mp/MpAudioConnection.h"
#include "mp/MpdGIPSiPCMA.h"
#include "mp/GIPS/GIPS_API.h"
#include "mp/MprDejitter.h"
const MpCodecInfo MpdGIPSiPCMA::smCodecInfo(
         SdpCodec::SDP_CODEC_GIPS_IPCMA, GIPS_API_VERSION, true,
         8000, 0, 1, 160, 64000, 176, 1280, 2240, 160);
MpdGIPSiPCMA::MpdGIPSiPCMA(int payloadType)
   : MpDecoderBase(payloadType, &smCodecInfo),
     pDecoderState(NULL)
{
   osPrintf("MpdGIPSiPCMA::MpdGIPSiPCMA(%d)\n", payloadType);
}

MpdGIPSiPCMA::~MpdGIPSiPCMA()
{
   freeDecode();
}

OsStatus MpdGIPSiPCMA::initDecode(MpAudioConnection* pConnection)
{
   int res = 0;

   //Get NetEq pointer
   mpJBState = pConnection->getJBinst();
   assert(NULL != mpJBState);

   //Allocate memory, only once though
   if (NULL == pDecoderState) {
      res += EG711A_GIPS_10MS16B_create(&pDecoderState);
   }

   // Set the payload number for NetEq
   NETEQ_GIPS_10MS16B_initCodepoint(mpJBState,
                                        "EG711A", 8000, getPayloadType());

   //Attach the decoder to NetEq instance
   res += NETEQEG711A_GIPS_10MS16B_init(mpJBState, pDecoderState);

   osPrintf("MpdGIPSiPCMA::initDecode: payloadType=%d\n", getPayloadType());
   return ((0==res) ? OS_SUCCESS : OS_NO_MEMORY);
}

OsStatus MpdGIPSiPCMA::freeDecode(void)
{
   int res;
   OsStatus ret = OS_DELETED;

   if (NULL != pDecoderState) {
      res = EG711A_GIPS_10MS16B_free(pDecoderState);
      pDecoderState = NULL;
      ret = OS_SUCCESS;
   }
   return ret;
}
#endif /* HAVE_GIPS ] */
