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

#include "utl/UtlListIterator.h"
#include "utl/UtlList.h"
#include "utl/UtlContainable.h"
#include "os/OsLock.h"


// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS

/*
 * The NOWHERE UtlLink is used when the iterator steps off the end
 * of the list [next() returns NULL]; mpCurrentNode is set to point
 * to this special stub empty list (using OFF_LIST_END for clarity).
 * If mpCurrentNode were left equal to NULL, then a subsequent call to
 * the iterator would return the first element on the list, because NULL
 * designates the state "before the first element".
 */
const UtlLink* UtlListIterator::OFF_LIST_END;

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Constructor
UtlListIterator::UtlListIterator(const UtlList& list)
   : UtlIterator(list),
     mpCurrentNode(NULL)
{
   OsLock container(const_cast<OsBSem&>(list.mContainerLock));
   
   addToContainer(&list);
}


// Destructor
UtlListIterator::~UtlListIterator()
{
   UtlContainer::acquireIteratorConnectionLock();
   OsLock take(mContainerRefLock);

   UtlList* myList = static_cast<UtlList*>(mpMyContainer);
   if (myList != NULL)
   {
      OsLock take(myList->mContainerLock);
      UtlContainer::releaseIteratorConnectionLock(); // as soon as both object locks are taken

      myList->removeIterator(this);
      mpMyContainer = NULL;
   }
   else
   {
      UtlContainer::releaseIteratorConnectionLock();
   }
}

/* ============================ MANIPULATORS ============================== */

// Assignment operator


// Return the next element .
UtlContainable* UtlListIterator::operator()()
{
   UtlContainable* nextVal = NULL;

   UtlContainer::acquireIteratorConnectionLock();
   OsLock take(mContainerRefLock);
   UtlList* myList = static_cast<UtlList*>(mpMyContainer);

   OsLock container(myList->mContainerLock);
   UtlContainer::releaseIteratorConnectionLock();

   mpCurrentNode = (mpCurrentNode == NULL
                    ? myList->head()
                    : mpCurrentNode->next()
                    );

   if(mpCurrentNode) // not end of list
   {
      nextVal = (UtlContainable*) mpCurrentNode->data;
   }
   else
   {
      // reset position so that subsequent calls will also return end of list
      mpCurrentNode = const_cast<UtlLink*>(OFF_LIST_END);
   }
    
   return(nextVal);
}


// Reset the list iterator.
void UtlListIterator::reset()
{
   UtlContainer::acquireIteratorConnectionLock();
   OsLock take(mContainerRefLock);
   UtlList* myList = static_cast<UtlList*>(mpMyContainer);
   OsLock container(myList->mContainerLock);
   UtlContainer::releaseIteratorConnectionLock();

   mpCurrentNode = NULL;
}


UtlContainable* UtlListIterator::toLast() 
{
   UtlContainable* last = NULL;
   
   UtlContainer::acquireIteratorConnectionLock();
   OsLock take(mContainerRefLock);
   UtlList* myList = static_cast<UtlList*>(mpMyContainer);
   OsLock container(myList->mContainerLock);
   UtlContainer::releaseIteratorConnectionLock();

   mpCurrentNode = myList->tail();
   last = static_cast<UtlContainable*>(mpCurrentNode ? mpCurrentNode->data : NULL);

   return last;
}


/* ============================ ACCESSORS ================================= */

// return the current data
UtlContainable* UtlListIterator::item() const
{
   UtlContainable* currentItem = NULL;

   UtlContainer::acquireIteratorConnectionLock();
   OsLock take(const_cast<OsBSem&>(mContainerRefLock));

   UtlList* myList = static_cast<UtlList*>(mpMyContainer);
   OsLock container(myList->mContainerLock);
   UtlContainer::releaseIteratorConnectionLock();

   currentItem = static_cast<UtlContainable*>(mpCurrentNode->data);

   return currentItem;
}

/* ============================ INQUIRY =================================== */

// Is the iterator positioned at the last element? 
UtlBoolean UtlListIterator::atLast() const 
{
   UtlBoolean isAtLast = false;
   
   UtlContainer::acquireIteratorConnectionLock();

   OsLock take(const_cast<OsBSem&>(mContainerRefLock));
   UtlList* myList = static_cast<UtlList*>(mpMyContainer);
   OsLock container(myList->mContainerLock);
   UtlContainer::releaseIteratorConnectionLock();
   
   isAtLast = (mpCurrentNode && mpCurrentNode == myList->tail());

   return isAtLast;
}       

/* //////////////////////////// PROTECTED ///////////////////////////////// */


/**
 * removing is called by the UtlList when an element is about to be
 * removed from the container.  The iterator must ensure that the removed
 * element is not returned by any subsequent call.
 */
void UtlListIterator::removing(const UtlLink* node)
{
   // The caller already holds the mContainerLock.
   if (mpCurrentNode == node)
   {
      mpCurrentNode = node->prev();
   }
}



/* //////////////////////////// PRIVATE /////////////////////////////////// */

/* ============================ FUNCTIONS ================================= */
