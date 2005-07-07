// 
//
// Copyright (c) PingTel Corp. (work in progress)
//
// This is an unpublished work containing PingTel Corporation's confidential
// and proprietary information.  Disclosure, use or reproduction without
// written authorization of PingTel Corp. is prohibited.
//
//! lib=mailbox
//////////////////////////////////////////////////////////////////////////////
#ifndef DIALBYNAME_H
#define DIALBYNAME_H

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include "net/Url.h"
#include "mailboxmgr/CGICommand.h"

// DEFINES
#define DIALBYNAME_MAX_BLOCKSIZE 9
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
 * Mailbox Class
 *
 * @author John P. Coffey
 * @version 1.0
 */
class DialByNameCGI : public CGICommand
{
public:
    /**
     * Ctor
     */
    DialByNameCGI( const Url& from,
                   const UtlString& digits,
                   const UtlBoolean& invokedByDeposit);

    /**
     * Virtual Dtor
     */
    virtual ~DialByNameCGI();

    /** This does the work */
    virtual OsStatus execute (UtlString* out = NULL);

protected:

private:

	/** SIP URL of the caller */
	const Url m_from;

	/** Digits dialed by the user while entering the name */
	const UtlString m_digits;

    /** DialByName CGI can be invoked from two places:
     *  -   m_invokedByDepositFlag = FALSE --> from the autoattendant 
     *      wherein the user wants to connect to the extension he dialed. 
     *      In this case, transfer to the extension.
     *  -   m_invokedByDepositFlag = FALSE --> From deposit CGI 
     *      where the user wants to send the message he recorded to 
     *      the extension he dialed. In this case, return the fully qualified 
     *      mailbox identity of the extension to the calling vxml.
     */
    const UtlBoolean m_invokedByDepositFlag ;
};

#endif //DIALBYNAME_H

