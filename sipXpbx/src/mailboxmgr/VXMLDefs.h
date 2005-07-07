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

#ifndef VXMLDEFS_H
#define VXMLDEFS_H

// SYSTEM INCLUDES
//#include <...>
#include "os/OsStatus.h"
#include "os/OsDefs.h"



// APPLICATION INCLUDES
#include "mailboxmgr/CGICommand.h"

#define VXML_HEADER "Content-type: text/vxml\n" \
    "pragma: no-cache\n\n"
					 
#define VXML_BODY_BEGIN	"<?xml version=\"1.0\"?>\n" \
    "<vxml version=\"2.0\">\n" 
    
#define VXML_BEGIN_WITH_ROOT "<?xml version=\"1.0\"?>\n" \
    "<vxml version=\"2.0\" application=\""
    
// @JC HACK HERE to put Trailing \n
#define VXML_END "</vxml>\n"

#define VXML_SUCCESS_SNIPPET "<form> <block>\n" \
	 VXML_SUCCESS_BLOCK \
    "</block> </form>\n"
								
#define VXML_FAILURE_SNIPPET "<form> <block>\n" \
	VXML_FAILURE_BLOCK \
    "</block> </form>\n"

#define VXML_INVALID_EXTN_SNIPPET "<form> <block>\n" \
	VXML_INVALID_EXTN_BLOCK \
    "</block> </form>\n"	
		
#define VXML_INVALID_EXTN_BLOCK "<var name=\"result\" expr=\"'invalidextn'\"/>\n" \
	"<return namelist=\"result\"/>\n" 
	 
#define VXML_SUCCESS_BLOCK "<var name=\"result\" expr=\"'success'\"/>\n" \
    "<return namelist=\"result\"/>\n" 
								
#define VXML_FAILURE_BLOCK "<var name=\"result\" expr=\"'failed'\"/>\n" \
	"<return namelist=\"result\"/>\n" 

#define VXML_TIMEOUT_PROPERTY_START "<property name=\"timeout\" value=\""
#define VXML_TIMEOUT_PROPERTY_END	"\"/>\n"\

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

#endif //VXMLDEFS_H

