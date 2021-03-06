//
// Copyright (C) 2006-2012 SIPez LLC.  All rights reserved.
//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES

#include <string.h>
#include <ctype.h>
#include <stdio.h>

// APPLICATION INCLUDES
#include "net/NameValuePairInsensitive.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS

const UtlContainableType NameValuePairInsensitive::TYPE =
    "NameValuePairInsensitive";

// Most methods are carried over from NameValuePair.

// Constructor
NameValuePairInsensitive::NameValuePairInsensitive(const char* name,
                                                   const char* value) :
   NameValuePair(name, value)
{
   // All the work is done by the NameValuePair constructor.
}

// Copy constructor
NameValuePairInsensitive::NameValuePairInsensitive(
   const NameValuePairInsensitive& rNameValuePairInsensitive) :
   NameValuePair(rNameValuePairInsensitive)
{
   // All the work is done by the NameValuePair constructor.
}

// Destructor
NameValuePairInsensitive::~NameValuePairInsensitive()
{
   // All the work is done by the NameValuePair destructor.
}

/* ============================ MANIPULATORS ============================== */

// Assignment operator
NameValuePairInsensitive&
NameValuePairInsensitive::operator=(const NameValuePairInsensitive& rhs)
{
   ((NameValuePair&) *this) = (const NameValuePair) rhs;

   return *this;
}

/* ============================ ACCESSORS ================================= */
const char* NameValuePairInsensitive::getValue()
{
   return ((NameValuePair*) this)->getValue();
}

void NameValuePairInsensitive::setValue(const char* newValue)
{
   ((NameValuePair*) this)->setValue(newValue);
}

/* ============================ INQUIRY =================================== */

// Returns a hash value. the algorithm is according to
// g_string_hash() in glib.
unsigned NameValuePairInsensitive::hash() const
{
    // Need to use data() in case mpData is null
    const char* pHashData = data();
    size_t hashSize = length();
    unsigned hashValue = 0;

    while (hashSize > 0)
    {
       // Put each character through toupper before hashing it.
       hashValue = (hashValue << 5) - hashValue + toupper(*pHashData);
       pHashData++;
       hashSize--;
    }

    return hashValue;
}


// Get the ContainableType for a UtlContainable derived class.
UtlContainableType NameValuePairInsensitive::getContainableType() const
{
    return NameValuePairInsensitive::TYPE;
}

// Compare this object to another like-objects.
int
NameValuePairInsensitive::compareTo(
   UtlContainable const * compareContainable) const
{
    int compareFlag = -1;

    if (compareContainable)
    {
       compareFlag =
          ((UtlString*) this)->
          compareTo(((UtlString*) compareContainable)->data(),
                    UtlString::ignoreCase);
    }

    return compareFlag;
}


// Test this object to another like-object for equality.
UtlBoolean
NameValuePairInsensitive::isEqual(
   UtlContainable const * compareContainable) const
{
    return (compareTo(compareContainable) == 0);
}

UtlBoolean NameValuePairInsensitive::isInstanceOf(const UtlContainableType type) const
{
    // Check if it is my type and the defer parent type comparisons to parent
    return(areSameTypes(type, NameValuePairInsensitive::TYPE) ||
           NameValuePair::isInstanceOf(type));
}

/* //////////////////////////// PROTECTED ///////////////////////////////// */

/* //////////////////////////// PRIVATE /////////////////////////////////// */

/* ============================ FUNCTIONS ================================= */
