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
// APPLICATION INCLUDES

// Keep UtlInit.h as the first include!
// See UtlInit class description for more information.
#include "utl/UtlInit.h"

#include "utl/UtlContainer.h"
#include "utl/UtlLink.h"
#include "utl/UtlIterator.h"
#include "os/OsLock.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
const UtlContainableType UtlContainer::TYPE = "UtlContainer";

// STATIC VARIABLE INITIALIZATIONS

OsBSem* UtlContainer::spIteratorConnectionLock;


/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Constructor

UtlContainer::UtlContainer()
   : mContainerLock(OsBSem::Q_PRIORITY, OsBSem::FULL)
{
}


// Destructor
UtlContainer::~UtlContainer() 
{
}

// invalidateIterators() isn't called by the UtlContainer destructor
// (and need not be, since no iterators are defined for UtlContainer),
// but it is used by the methods for various subclasses.
void UtlContainer::invalidateIterators()
{
   UtlLink*        listNode;
   UtlIterator*    foundIterator;

   // The caller is holding the sIteratorConnectionLock and mContainerLock.
       
   // Walk the list to notify the iterators.
   for (listNode = static_cast<UtlLink*>(mIteratorList.head());
        listNode != NULL;
        listNode = listNode->next()
        )
   {
      foundIterator = (UtlIterator*)listNode->data;
      foundIterator->invalidate();
   }

   assert(mIteratorList.isUnLinked());// some iterator failed to call removeIterator
}

/* ============================ MANIPULATORS ============================== */




/* ============================ ACCESSORS ================================= */

// These are here because every UtlContainer is also a UtlContainable

/**
 * Calculate a unique hash code for this object.  If the equals
 * operator returns true between two objects, then both objects
 * must have the same hash code.
 */
unsigned UtlContainer::hash() const
{
   // default implementation
   return (uintptr_t) this;
}


/**
 * Get the ContainableType for a UtlContainable derived class.
 */
UtlContainableType UtlContainer::getContainableType() const
{
   return UtlContainer::TYPE;
}

/* ============================ INQUIRY =================================== */


/**
 * Compare the this object to another like object.  Results for 
 * comparing with a non-like object are undefined.
 *
 * @returns 0 if equal, <0 if less than and >0 if greater.
 */
int UtlContainer::compareTo(const UtlContainable* otherObject) const
{
   return ((uintptr_t) this) - ((uintptr_t) otherObject);
}


/* //////////////////////////// PROTECTED ///////////////////////////////// */

/// Lock the linkage between containers and iterators
void UtlContainer::acquireIteratorConnectionLock()
{
   spIteratorConnectionLock->acquire();
}

/// Unlock the linkage between containers and iterators
void UtlContainer::releaseIteratorConnectionLock()
{
   spIteratorConnectionLock->release();
}

void UtlContainer::addIterator(UtlIterator* newIterator) const
{
   // Caller must be holding this->mContainerLock.
   // But it need not be holding newIterator->mpContainerRefLock, because
   // we do not set newIterator->mpMyContainer.

   // This method is declared const because it makes no change that
   // any other method can detect in the container, but it actually
   // does make a change, so we have to cast away the const.
   UtlContainer* my = const_cast<UtlContainer*>(this);
   
    if(newIterator)
    {
       // :HACK: note that we are storing a UtlIterator* in the UtlContainer* pointer
       UtlLink* iteratorLink = UtlLink::get();
       iteratorLink->data = (UtlContainer*)newIterator;
       iteratorLink->UtlChain::listBefore(&my->mIteratorList, NULL);
    }
}


void UtlContainer::removeIterator(UtlIterator *existingIterator) const
{
   // Caller must be holding this->mContainerLock.
   // But it need not be holding newIterator->mpContainerRefLock, because
   // we do not set newIterator->mpMyContainer.

   // This method is declared const because it makes no change that
   // any other method can detect in the container, but it actually
   // does make a change, so we have to cast away the const.
   
   if(existingIterator)
   {
      UtlContainer *my = (UtlContainer*)this;
      UtlLink* iteratorLink;

      // :HACK: note that we are storing a UtlIterator* in the UtlContainer* pointer
      iteratorLink = UtlLink::findData(&my->mIteratorList, (UtlContainer*)existingIterator);
      if (iteratorLink)
      {
         iteratorLink->detachFrom(&my->mIteratorList);
      }
   }
}
