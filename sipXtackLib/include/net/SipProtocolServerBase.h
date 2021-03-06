//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _SipProtocolServerBase_h_
#define _SipProtocolServerBase_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <net/SipClient.h>
#include <os/OsServerSocket.h>
#include <os/OsTask.h>
#include <os/OsServerTask.h>
#include <os/OsLockingList.h>
#include <os/OsRWMutex.h>
#include <utl/UtlHashMap.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class SipUserAgent;
class SipServerBrokerListener;

//:Class short description which may consist of multiple lines (note the ':')
// Class detailed description which may extend to multiple lines
class SipProtocolServerBase : public OsTask
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

   SipProtocolServerBase(SipUserAgent* userAgent,
                         const char* protocolString,
                         const char* taskName);
     //:Default constructor


   virtual
   ~SipProtocolServerBase();
     //:Destructor

/* ============================ MANIPULATORS ============================== */

    virtual UtlBoolean startListener();

    virtual void shutdownListener() = 0;


    UtlBoolean send(SipMessage* message, const char* hostAddress,
            int hostPort = SIP_PORT);

    virtual int run(void* pArg) = 0;

    void removeOldClients(long oldTime);

/* ============================ ACCESSORS ================================= */

    int getClientCount();

    virtual void printStatus();

    virtual UtlBoolean isOk();

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
    friend class SipServerBrokerListener;

    SipClient* createClient(const char* hostAddress,
                            int hostPort,
                            const char* localIp);

    void releaseClient(SipClient* client);

    void startClients();

    void shutdownClients();

    UtlBoolean clientExists(SipClient* client);

    void addClient(SipClient* client);

    virtual OsSocket* buildClientSocket(int hostPort, const char* hostAddress, const char* localIp) = 0;

    UtlString mProtocolString;
    UtlString mDefaultIp;
    int mDefaultPort;
    SipUserAgent* mSipUserAgent;
    UtlHashMap mServerSocketMap;
    UtlHashMap mServerPortMap;
    UtlHashMap mServers;
    SipServerBrokerListener* mpServerBrokerListener;
    


/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

    UtlBoolean waitForClientToWrite(SipClient* client);

    SipClient* getClient(const char* hostAddress,
                         int hostPort,
                         const char* localIp);

    void deleteClient(SipClient* client);

        OsRWMutex mClientLock;
    OsLockingList mClientList;

        SipProtocolServerBase(const SipProtocolServerBase& rSipProtocolServerBase);
        //: disable Copy constructor

   SipProtocolServerBase& operator=(const SipProtocolServerBase& rhs);
     //:disable Assignment operator

};

/* ============================ INLINE METHODS ============================ */

#endif  // _SipProtocolServerBase_h_
