//
// Copyright (C) 2006-2011 SIPez LLC.  All rights reserved.
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

#ifndef _UTLBOOL_H_
#define _UTLBOOL_H_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include "utl/UtlDefs.h"
#include "utl/UtlCopyableContainable.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
 * UtlBool is a UtlContainable wrapper for a bool.
 */
class UtlBool : public UtlCopyableContainable
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

    static const UtlContainableType TYPE ;    /** < Class type used for runtime checking */ 

/* ============================ CREATORS ================================== */

    /**
     * Constructor
     */
    UtlBool(bool bValue) ;
      
    /**
     * Destructor
     */
    virtual ~UtlBool();

    /// Make a copy of this
    virtual UtlCopyableContainable* clone() const;

/* ============================ MANIPULATORS ============================== */

    /**
     * Set a new bool value for this object.
     *
     */
    void setValue(bool bValue) ;

/* ============================ ACCESSORS ================================= */

    /**
     * Get the bool wrapped by this object.
     */
    bool getValue() const ;    

    /**
     * Calculate a unique hash code for this object.  If the equals
     * operator returns true for another object, then both of those
     * objects must return the same hashcode.
     */
    virtual unsigned hash() const ;

    /**
     * Get the ContainableType for a UtlContainable derived class.
     */
    virtual UtlContainableType getContainableType() const;
         
/* ============================ INQUIRY =================================== */

    /**
     * Compare the this object to another like-objects.  Results for 
     * designating a non-like object are undefined.
     *
     * @returns 0 if equal, < 0 if less then and >0 if greater.
     */
    virtual int compareTo(UtlContainable const *) const ;    

    /// @copydoc UtlContainable::isInstanceOf
    virtual UtlBoolean isInstanceOf(const UtlContainableType type) const;

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
    bool mValue ;    /** < The bool wrapped by this object */ 

} ;

/* ============================ INLINE METHODS ============================ */

#endif    // _UTLBOOL_H_
