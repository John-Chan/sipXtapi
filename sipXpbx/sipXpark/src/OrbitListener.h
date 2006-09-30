// 
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
////////////////////////////////////////////////////////////////////////////

#ifndef _OrbitListener_h_
#define _OrbitListener_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include <tao/TaoAdaptor.h>
#include <ParkedCallObject.h>
#include <utl/UtlHashMap.h>

// DEFINES
#define ORBIT_CONFIG_FILE     "orbits.xml"

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class CallManager;
class TaoString;

//:Class short description which may consist of multiple lines (note the ':')
// Class detailed description which may extend to multiple lines
class OrbitListener : public TaoAdaptor
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

   OrbitListener(CallManager* callManager = NULL);

   //:Default constructor
   ~OrbitListener();

/* ============================ MANIPULATORS ============================== */

   UtlBoolean handleMessage(OsMsg& rMsg);
     //: Method to process messages which get queued on this
     /**  The is the method that does all the work.  Telephony
       *  events get queued on this which consequently get
       *  processed in handleMessage in the contect of this
       *  task.
       */

/* ============================ ACCESSORS ================================= */

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
    void dumpTaoMessageArgs(unsigned char eventId, TaoString& args);

    ParkedCallObject* findEntry(UtlString& rKey);
    void insertEntry(UtlString& callId, ParkedCallObject* call);
    ParkedCallObject* removeEntry(UtlString& callId);
    unsigned int validateOrbitRequest(UtlString& callId, UtlString& address, UtlString& audio);
    bool isCallRetrievalInvite(const char* callId, const char* address);


/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
    UtlString mPlayfile;
    
    OrbitListener(const OrbitListener& rOrbitListener);
     //:Copy constructor

    OrbitListener& operator=(const OrbitListener& rOrbitListener);
     //:Assignment operator
     
    void getOrbit(UtlString& callId, UtlString& address, UtlString &orbit);
    ParkedCallObject* getOldestCallInOrbit(UtlString& orbit, UtlString& callId, UtlString& address);

    CallManager* mpCallManager;

    UtlHashMap mCalls;
};

/* ============================ INLINE METHODS ============================ */

#endif  // _OrbitListener_h_
