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


#ifndef _StreamDefs_h_
#define _StreamDefs_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES

// DEFINES
#define STREAM_SOUND_LOCAL  0x00000002 ///< Play the sound locally
#define STREAM_SOUND_REMOTE 0x00000004 ///< Play the sound remotely

#define STREAM_FORMAT_AUTO  0x00010000 ///< Auto detect data format
#define STREAM_FORMAT_RAW   0x00020000 ///< Force RAW format
#define STREAM_FORMAT_WAV   0x00040000 ///< Force WAV Format
#define STREAM_FORMAT_AU    0x00080000 ///< Force AU Format
#define STREAM_FORMAT_MP3   0x00100000 ///< Force MP3 Format

#define STREAM_HINT_CACHE   0x10000000 ///< Cache entire file

#undef  MP_STREAM_DEBUG
//#define  MP_STREAM_DEBUG

// MACROS
// CONSTANTS
// FORWARD DECLARATIONS
// STRUCTS
// TYPEDEFS
  /// Handles used by the renderer
typedef void * StreamHandle ;

typedef enum
{
   FeederRealizedEvent,        ///< Data has been realized
   FeederPrefetchedEvent,      ///< Data has been prefetched
   FeederRenderingEvent,       ///< Rendering has begun
   FeederStoppedEvent,         ///< Rendering has stopped
   FeederFailedEvent,          ///< Rendering has failed

   FeederStreamPlayingEvent,   ///< Playing has begun
   FeederStreamPausedEvent,    ///< Playing has paused
   FeederStreamStoppedEvent,   ///< Playing has completed
   FeederStreamDestroyedEvent, ///< Resource has been destroyed
   FeederStreamAbortedEvent    ///< Aborted by User

} FeederEvent ;

// EXTERNAL VARIABLES
// EXTERNAL FUNCTIONS
// FUNCTIONS


#endif  // _StreamDefs_h_

