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

#ifndef DISABLE_STREAM_PLAYER // [

#ifndef _MprFromStream_h_
#define _MprFromStream_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "mp/dtmflib.h"
#include "mp/MpFlowGraphMsg.h"
#include "mp/MpAudioResource.h"
#include "mp/StreamDefs.h"

#include "mp/MpStreamFeeder.h"
#include "os/OsLockingList.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
typedef struct tagSTREAMDESC
{
    StreamHandle    handle ;
    MpStreamFeeder* pFeeder ;

} STREAMDESC ;
// FORWARD DECLARATIONS


/// @brief This resource is an insertion point for the streaming 
/// infrastructure by connecting the flowgraph to MpStreamFeeders.
/**
*  Whenever requested to "realize" a stream, this code will create a 
*  MpStreamFeeder which inturn creates a StreamDataSource and a 
*  StreamFormatDecoder.  Many MpStreamFeeders may be managed/created by
*  a single MprFromStream, however, only one can be active at only one
*  point.
*  
*  For thread safety, all operations (play, destroy, rewind, etc.) 
*  result in a message being posted to itself.  This forces all of the
*  tasks to be performed on the same task context.  The "realize" 
*  operation is an exception, where it is processed on the called 
*  context.
* 
*  Additionally, the code creates a level of indirection between between
*  the implementation and calling parties (application layer).  All 
*  MpStreamFeeders created by this resource are collected within a list 
*  (mStreamList) and only a handle to the resource is exposed to 
*  application layer.  Handles are unique for an instance of a
*  MprFromStream.  If the destruct is called for MprFromStream and the
*  mStreamList is not null, the list is purged.
* 
* 
*  This class bubbles events to the MpStreamFeeder by calling the 
*  fromStreamUpdate callback on the feeder itself.
*/
class MprFromStream : public MpAudioResource
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

   MprFromStream(const UtlString& rName);
     //:Constructor

   virtual
   ~MprFromStream();
     //:Destructor

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

   OsStatus realize(Url urlSource,
                    int flags,
                    StreamHandle &handle,                    
                    OsNotification* event = NULL);

   OsStatus realize(UtlString* pBuffer,
                    int flags,
                    StreamHandle &handle,
                    OsNotification* event = NULL);

   OsStatus prefetch(StreamHandle handle);

   OsStatus play(StreamHandle handle);

   OsStatus rewind(StreamHandle handle);

   OsStatus pause(StreamHandle handle);
   
   OsStatus stop(StreamHandle handle) ;

   OsStatus destroy(StreamHandle handle) ;

   OsStatus getFlags(StreamHandle handle, int& flags) ;

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
   OsStatus setStreamSource(MpStreamFeeder *pFeeder) ;
   MpStreamFeeder* getStreamSource() ;

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
   typedef enum
   {     
      SOURCE_PLAY = MpFlowGraphMsg::RESOURCE_SPECIFIC_START,
      SOURCE_PAUSE,
      SOURCE_REWIND,
      SOURCE_STOP,
      SOURCE_DESTROY,
      SOURCE_RENDER
   } AddlMsgTypes;


   OsNotification* mpNotify;
   MpStreamFeeder* mpStreamRenderer;
   FeederEvent     mEventState;
   UtlBoolean      mbStreamChange;
   int             miStreamCount;      ///< Count for generating unique handles
   OsLockingList   mStreamList;        ///< List of stream players

   virtual UtlBoolean doProcessFrame(MpBufPtr inBufs[],
                                     MpBufPtr outBufs[],
                                     int inBufsSize,
                                     int outBufsSize,
                                     UtlBoolean isEnabled,
                                     int samplesPerFrame,
                                     int samplesPerSecond);

     /// Handle messages for this resource.
   virtual UtlBoolean handleMessage(MpFlowGraphMsg& rMsg);

   UtlBoolean handleRender(MpStreamFeeder* pFeeder);
   UtlBoolean handlePlay(MpStreamFeeder* pFeeder);
   UtlBoolean handleRewind(MpStreamFeeder* pFeeder);
   UtlBoolean handlePause(MpStreamFeeder* pFeeder);
   UtlBoolean handleStop(MpStreamFeeder* pFeeder);
   UtlBoolean handleDestroy(MpStreamFeeder* pFeeder);
   

     /// Copy constructor (not implemented for this class)
   MprFromStream(const MprFromStream& rMprFromStream);

     /// Assignment operator (not implemented for this class)
   MprFromStream& operator=(const MprFromStream& rhs);

     /// Get the stream feeder for the given handle
   MpStreamFeeder* getStreamFeeder(StreamHandle handle) ;

     /// Removes the stream feeder from the stream list.
   MpStreamFeeder* removeStreamFeeder(StreamHandle handle) ;
     /**<
     *  The Stream feeder is returned if found, someone else is responsible for
     *  deleting it.
     */

     /// Stops, destroys, and frees all stream feeders
   void destroyFeeders() ;

};

/* ============================ INLINE METHODS ============================ */

#endif  /* _MprFromStream_h_ */

#endif // DISABLE_STREAM_PLAYER ]
