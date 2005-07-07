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

#ifndef ManageFoldersWebCGI_H
#define ManageFoldersWebCGI_H

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include "utl/UtlString.h"
#include "mailboxmgr/CGICommand.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
 * ManageFoldersWebCGI Class
 *
 * @author Harippriya M Sivapatham
 * @version 1.0
 */
class ManageFoldersWebCGI : public CGICommand
{
public:
    /**
     * Ctor
     */
    ManageFoldersWebCGI (       const UtlString& mailboxIdentity,
                            const UtlString& action,
                            const UtlString& status,
                            const UtlString& strNewFolderName,
                            const UtlString& strOldFolderName
                        );

    /**
     * Virtual Dtor
     */
    virtual ~ManageFoldersWebCGI();

    /** This does the work */
    virtual OsStatus execute ( UtlString* out = NULL );

    OsStatus getAllFolders(UtlString* out) ;

    OsStatus getPersonalFolders(UtlString* out) ;

    OsStatus addEditDeleteFolder(UtlString* out) ;

    OsStatus getAddEditFolderUI(UtlString* out) ;

protected:

private:
        UtlString m_mailboxIdentity;
    const UtlString m_action ;
    const UtlString m_status ;
    UtlString m_newFolder ;
    UtlString m_oldFolder ;
    UtlString m_cgiUrl ;
};

#endif //ManageFoldersWebCGI_H
