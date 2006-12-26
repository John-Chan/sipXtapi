//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

#ifndef _MprToneGen_h_
#define _MprToneGen_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "mp/dtmflib.h"
#include "mp/MpFlowGraphMsg.h"
#include "mp/MpAudioResource.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
*  @brief The "Tone Generator" media processing resource
*
*/
class MprToneGen : public MpAudioResource
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// Constructor
   MprToneGen(const UtlString& rName, int samplesPerFrame, int samplesPerSec,
              const char* locale);

     /// Destructor
   virtual
   ~MprToneGen();

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

#ifdef LATER
/* Later (soon) this will be incorporated, but this is not quite the right
implementation.  At least these changes are needed:
(1) this should be an overriding virtual function, named
    handleSetSamplesPerSec.
(2) MpResource (the base class) needs to be enhanced so that the base
    virtual function exists to be overridden.
*/

     /// Sets the number of samples expected per second.
   virtual UtlBoolean setSamplesPerSec(int samplesPerSec);
     // Returns FALSE if the specified rate is not supported, TRUE otherwise.
#endif

     /// @brief Sends a START_TONE message to this resource to begin generating 
     /// an audio tone.
   OsStatus startTone(int toneId);
     /**<
     *  Returns the result of attempting to queue the message to this resource.
     */

     /// @brief Sends a STOP_TONE message to this resource to stop generating 
     /// an audio tone.
   OsStatus stopTone(void);
     /**<
     *  Returns the result of attempting to queue the message to this resource.
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

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
   typedef enum
   {
      START_TONE = MpFlowGraphMsg::RESOURCE_SPECIFIC_START,
      STOP_TONE
   } AddlMsgTypes;

   static const int MIN_SAMPLE_RATE;
   static const int MAX_SAMPLE_RATE;

   MpToneGenPtr mpToneGenState;

   virtual UtlBoolean doProcessFrame(MpBufPtr inBufs[],
                                     MpBufPtr outBufs[],
                                     int inBufsSize,
                                     int outBufsSize,
                                     UtlBoolean isEnabled,
                                     int samplesPerFrame,
                                     int samplesPerSecond);

     /// Handle messages for this resource.
   virtual UtlBoolean handleMessage(MpFlowGraphMsg& rMsg);

     /// Copy constructor (not implemented for this class)
   MprToneGen(const MprToneGen& rMprToneGen);

     /// Assignment operator (not implemented for this class)
   MprToneGen& operator=(const MprToneGen& rhs);

};

/* ============================ INLINE METHODS ============================ */

#endif  // _MprToneGen_h_
