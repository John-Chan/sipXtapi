// 
// 
// Copyright (C) 2005 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2005 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef SIPREDIRECTORREGDB_H
#define SIPREDIRECTORREGDB_H

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include "SipRedirector.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
 * SipRedirectorRegDB is singleton class whose object adds contacts that are
 * listed in the registration database.
 */

class SipRedirectorRegDB : public SipRedirector
{
  public:

   SipRedirectorRegDB();

   ~SipRedirectorRegDB();

   virtual OsStatus initialize(const UtlHashMap& configParameters,
                               OsConfigDb& configDb,
                               SipUserAgent* pSipUserAgent,
                               int redirectorNo);

   virtual void finalize();

   virtual SipRedirector::LookUpStatus lookUp(
      const SipMessage& message,
      const UtlString& requestString,
      const Url& requestUri,
      const UtlString& method,
      SipMessage& response,
      RequestSeqNo requestSeqNo,
      int redirectorNo,
      SipRedirectorPrivateStorage*& privateStorage);

  protected:
};

#endif // SIPREDIRECTORREGDB_H
