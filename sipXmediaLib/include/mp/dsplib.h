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

#ifndef __INCdsplib_h /* [ */
#define __INCdsplib_h

#include "mp/DSP_type.h"
#include "mp/MpBuf.h"
#include "mp/MpTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* comfort noise generator assume the max size of processing frame is 80 */
#define SAMPLES_PER_FRAME 80

// -----------------------------------------------------
// Comfort Noise Generator module used by MprDecode
// -----------------------------------------------------
extern void init_CNG();

extern void white_noise_generator(MpAudioSample  *shpSamples,
                                  int     iLength,
                                  Word32  ulNoiseLevelAve);

extern void comfort_noise_generator(MpAudioSample  *shpSamples,
                                    int     iLength,
                                    Word32  ulNoiseLevelAve);

extern void background_noise_level_estimation(Word32& shNoiseLevel,
                                              MpAudioSample* shpSamples,
                                              int     iLength);

//////////////////////////////////////////////////////////////////////////////

#ifdef NEVER_GOT_USED /* [ */
extern void dspCopy32Sto16S(const int* src, short* dst, int count);

extern void dspCopy16Sto32S(const short* src, int* dst, int count);

/* dot product of an array of 32 bit samples by 32 bit coefficients */
extern Word64S dspDotProd32x32(const int* v1, const int* v2,
                               int count, Word64S* res = 0);

#endif /* NEVER_GOT_USED ] */

#define dspDotProd32S dspDotProd16x32

/* dot product of an array of 16 bit samples by 32 bit coefficients */
extern Word64S dspDotProd16x32(const short* v1, const int* v2,
                               int count, Word64S* res = 0);

/* special version that skips every other item in v1 */
extern Word64S dspDotProd16skip32(const short* v1, const int* v2,
                                  int count, Word64S* res = 0);

/* Coefficient update routines */
extern void dspCoeffUpdate16x32(const short* v1, int* v2,
                               int count, int factor);

extern void dspCoeffUpdate16skip32(const short* v1, int* v2,
                               int count, int factor);


typedef struct 
   {
      int r;
      int i;
   } icomplex;

extern int imagsq(icomplex* x, int PreRightShift);

extern void complexInnerProduct(icomplex *ResultPtr,
               icomplex *CoeffPtr, icomplex *DLPtr, int EcIndex);

extern void complexCoefUpdate5(icomplex *NormedErrPtr,
               icomplex *CoeffPtr, icomplex *DLPtr, int EcIndex, int* Shifts);

#ifdef __cplusplus
}
#endif

#endif /* __INCdsplib_h ] */
