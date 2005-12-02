// 
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include "os/OsFS.h"
#include "utl/UtlString.h"
#include "utl/UtlHashMap.h"
#include "mailboxmgr/VXMLDefs.h"
#include "mailboxmgr/AutoAttendantCGI.h"
#include "mailboxmgr/MailboxManager.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/* ============================ CREATORS ================================== */

AutoAttendantCGI::AutoAttendantCGI( const Url& from, const UtlString& name, const char* digits ) :
   m_from ( from ),
   m_name ( name ),
   m_digits ( digits )
{
}

AutoAttendantCGI::~AutoAttendantCGI()
{
}

OsStatus
AutoAttendantCGI::execute(UtlString* out)
{
   // Get the base URL of mediaserver. This is necessary for playing prompts.
   MailboxManager* pMailboxManager = MailboxManager::getInstance();

   UtlString ivrPromptUrl;
   pMailboxManager->getIvrPromptURL( ivrPromptUrl );

   UtlString mediaserverUrl;
   MailboxManager::getInstance()-> getMediaserverURL( mediaserverUrl );

   UtlString secureMediaserverUrl;
   pMailboxManager->getMediaserverSecureURL( secureMediaserverUrl );

   // call helper method to convert string to friendly form suitable for
   // transfer to the OpenVXI engine
   UtlString vxmlFriendlyFrom = m_from.toString();
   MailboxManager::convertUrlStringToXML ( vxmlFriendlyFrom );

   // Construct the dynamic VXML
   UtlString dynamicVxml(VXML_BODY_BEGIN);
   dynamicVxml +=  "<form>\n";
   dynamicVxml += "<subdialog name=\"autoattendant\" src=\"" + mediaserverUrl + "/aa_vxml/autoattendant" + "-" + m_name + ".vxml\">\n";
   dynamicVxml += "<param name=\"from\" value=\"" + vxmlFriendlyFrom + "\"/>\n" \
      "<param name=\"mediaserverurl\" expr=\"'" + ivrPromptUrl + "'\"/>\n" \
      "<param name=\"securemediaserverurl\" expr=\"'" + secureMediaserverUrl + "'\"/>\n" \
      "</subdialog>\n" \
      "</form>\n";
   dynamicVxml += VXML_END;

   // Write out the dynamic VXML script to be processed by OpenVXI
   if (out) 
   {
      out->remove(0);
      UtlString responseHeaders;
      MailboxManager::getResponseHeaders(dynamicVxml.length(), responseHeaders);

      out->append(responseHeaders.data());
      out->append(dynamicVxml.data());
   }
   return OS_SUCCESS;
}


