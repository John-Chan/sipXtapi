//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////
// Author: Dan Petrie (dpetrie AT SIPez DOT com)

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include <os/OsLock.h>
#include <os/OsDateTime.h>
#include <os/OsTimer.h>
#include <os/OsEventMsg.h>
#include <utl/UtlHashMapIterator.h>
#include <net/SipRefreshManager.h>
#include <net/SipUserAgent.h>
#include <net/SipDialogMgr.h>
#include <net/SipDialog.h>

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS

// Private class to contain subscription client states
class RefreshDialogState : public UtlString
{
public:

    RefreshDialogState();

    virtual ~RefreshDialogState();
    void toString(UtlString& dumpString);

    // UtlString::data contains the Dialog Handle;
    void* mpApplicationData;
    SipRefreshManager::RefreshStateCallback mpStateCallback;
    int mExpirationPeriodSeconds; // original expiration
    long mPendingStartTime; // epoch time in seconds
    long mExpiration; // epoch time in seconds
    SipMessage* mpLastRequest;
    SipRefreshManager::RefreshRequestState mRequestState;
    int mFailedResponseCode;
    UtlString mFailedResponseText;
    OsTimer* mpRefreshTimer;  // Fires when it is time to resend

private:
    //! DISALLOWED accendental copying
    RefreshDialogState(const RefreshDialogState& rRefreshDialogState);
    RefreshDialogState& operator=(const RefreshDialogState& rhs);

};

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

RefreshDialogState::RefreshDialogState()
{
    mpApplicationData = NULL;
    mpStateCallback = NULL;
    mExpirationPeriodSeconds = -1;
    mPendingStartTime = -1;
    mExpiration = -1;
    mpLastRequest = NULL;
    mRequestState = SipRefreshManager::REFRESH_REQUEST_UNKNOWN;
    mFailedResponseCode = -1;
    mpRefreshTimer = NULL;
}

void RefreshDialogState::toString(UtlString& dumpString)
{
    dumpString = "RefreshDialogState:\n\tmpData: ";
    dumpString.append(*this);
    dumpString.append("\n\tmpApplicationData: ");
    char numBuf[20];
    sprintf(numBuf, "%p", mpApplicationData);
    dumpString.append(numBuf);
    dumpString.append("\n\tmpStateCallback: ");
    sprintf(numBuf, "%p", mpStateCallback);
    dumpString.append(numBuf);
    dumpString.append("\n\tmExpirationPeriodSeconds: ");
    sprintf(numBuf, "%d", mExpirationPeriodSeconds);
    dumpString.append(numBuf);
    dumpString.append("\n\tmPendingStartTime: ");
    sprintf(numBuf, "%ld", mPendingStartTime);
    dumpString.append(numBuf);
    dumpString.append("\n\tmExpiration: ");
    sprintf(numBuf, "%ld", mExpiration);
    dumpString.append(numBuf);
    dumpString.append("\n\tmpLastRequest: ");
    sprintf(numBuf, "%p", mpLastRequest);
    dumpString.append(numBuf);
    dumpString.append("\n\tmRequestState: ");
    UtlString stateString;
    SipRefreshManager::refreshState2String(mRequestState, stateString);
    dumpString.append(stateString);
    dumpString.append("\n\tmFailedResponseCode: ");
    sprintf(numBuf, "%d", mFailedResponseCode);
    dumpString.append(numBuf);
    dumpString.append("\n\tmFailedResponseText: ");
    dumpString.append(mFailedResponseText ? mFailedResponseText : "");
    dumpString.append("\n\tmpRefreshTimer: ");
    sprintf(numBuf, "%p", mpRefreshTimer);
    dumpString.append(numBuf);
}

// Copy constructor NOT ALLOWED
RefreshDialogState::RefreshDialogState(const RefreshDialogState& rRefreshDialogState)
{
}

RefreshDialogState::~RefreshDialogState()
{
}

//Assignment operator NOT ALLOWED
RefreshDialogState& 
RefreshDialogState::operator=(const RefreshDialogState& rhs)
{
   if (this == &rhs)            // handle the assignment to self case
      return *this;

   return *this;
}

// Constructor
SipRefreshManager::SipRefreshManager(SipUserAgent& userAgent, 
                                     SipDialogMgr& dialogMgr)
    : OsServerTask("SipRefreshManager-%d")
    , mRefreshMgrMutex(OsMutex::Q_FIFO)
{
    mpUserAgent = &userAgent;
    mpDialogMgr = &dialogMgr;
    mReceivingRegisterResponses = FALSE;
    mDefaultExpiration = 3600;
}

// Copy constructor
SipRefreshManager::SipRefreshManager(const SipRefreshManager& rSipRefreshManager)
: mRefreshMgrMutex(OsMutex::Q_FIFO)
{
    // NOT ALLOWED
}


// Destructor
SipRefreshManager::~SipRefreshManager()
{
    // Do not delete mpUserAgent ,mpDialogMgr.  They
    // may be used else where and need to be deleted outside the
    // SipRefreshManager.

    // Stop receiving SUBSCRIBE responses
    mpUserAgent->removeMessageObserver(*(getMessageQueue()));

    // Wait until this OsServerTask has stopped or handleMethod
    // might access something we are about to delete here.
    waitUntilShutDown();

    // Delete the event type strings
    mEventTypes.destroyAll();

    // Unsubscribe to anything that is in the list
    stopAllRefreshes();
    // mRefreshes should now be empty
}

/* ============================ MANIPULATORS ============================== */

// Assignment operator
SipRefreshManager& 
SipRefreshManager::operator=(const SipRefreshManager& rhs)
{
   if (this == &rhs)            // handle the assignment to self case
      return *this;

   return *this;
}

UtlBoolean SipRefreshManager::initiateRefresh(SipMessage& subscribeOrRegisterRequest,
                                               void* applicationData,
                                               const RefreshStateCallback refreshStateCallback,
                                               UtlString& earlyDialogHandle)
{

    UtlBoolean intitialRequestSent = FALSE;

    // Make sure we do not have an existing dialog or refresh session state
    // going for the given message
    UtlString messageDialogHandle;
    subscribeOrRegisterRequest.getDialogHandle(messageDialogHandle);
    UtlBoolean existingRefreshState = FALSE;
    UtlBoolean existingDialogState = FALSE;
    if(!SipDialog::isEarlyDialog(messageDialogHandle))
    {
        existingDialogState = TRUE;
        OsSysLog::add(FAC_SIP, PRI_ERR,
                "SipRefreshManager::initiateRefresh called with established dialog handle: %s",
                messageDialogHandle.data());
    }

    else
    {
        OsLock localLock(mRefreshMgrMutex);
        // See if there is an early or established dialog for this message
        if(getAnyDialog(messageDialogHandle))
        {
            existingRefreshState = TRUE;
            intitialRequestSent = FALSE;
            OsSysLog::add(FAC_SIP, PRI_ERR,
                "SipRefreshManager::initiateRefresh called with pre-existing refresh state: %s",
                messageDialogHandle.data());
        }

        // The dialog should not exist either
        else if(mpDialogMgr->dialogExists(messageDialogHandle) ||
            mpDialogMgr->earlyDialogExistsFor(messageDialogHandle))
        {
            existingDialogState = TRUE;
            OsSysLog::add(FAC_SIP, PRI_ERR,
                "SipRefreshManager::initiateRefresh called with pre-existing dialog: %s",
                messageDialogHandle.data());
        }
    }

    // Should not be any existing refresh or dialog states
    // for this message
    if(!existingRefreshState && !existingDialogState)
    {
        // Make sure we are registered to receive responses
        // for the message we are about to send
        UtlString method;
        subscribeOrRegisterRequest.getRequestMethod(&method);
        if(method.compareTo(SIP_REGISTER_METHOD) == 0)
        {
            lock();
            if(!mReceivingRegisterResponses)
            {
                mReceivingRegisterResponses = TRUE;
                // receive REGISTER responses 
                mpUserAgent->addMessageObserver(*(getMessageQueue()), 
                                                SIP_REGISTER_METHOD,
                                                FALSE, // yes requests
                                                TRUE, // no responses
                                                TRUE, // incoming,
                                                FALSE, // outgoing
                                                NULL);
            }
            unlock();
        }
        else if(method.compareTo(SIP_SUBSCRIBE_METHOD) == 0)
        {
            UtlString eventType;
            subscribeOrRegisterRequest.getEventField(&eventType, NULL);
            // Check to see if we have already registered to
            // receive the event type
            lock();
            if(mEventTypes.find(&eventType) == NULL)
            {
                mEventTypes.insert(new UtlString(eventType));
                // receive SUBSCRIBE responses for this event type
                mpUserAgent->addMessageObserver(*(getMessageQueue()), 
                                                SIP_SUBSCRIBE_METHOD,
                                                FALSE, // no requests
                                                TRUE, // yes responses
                                                TRUE, // incoming,
                                                FALSE, // outgoing
                                                eventType);
            }
            unlock();

        }

        // Create a new refresh state
        int requestedExpiration = 0;  // returned from following call
        RefreshDialogState* state = createNewRefreshState(subscribeOrRegisterRequest,
                                                        messageDialogHandle,
                                                        applicationData,
                                                        refreshStateCallback,
                                                        requestedExpiration);

        // create a new dialog
        mpDialogMgr->createDialog(subscribeOrRegisterRequest, 
                                  TRUE, // message from this side
                                  messageDialogHandle);

        // Keep track of when we send this request to be refreshed
        long now = OsDateTime::getSecsSinceEpoch();
        state->mPendingStartTime = now;
        state->mRequestState = REFRESH_REQUEST_PENDING;

        // Set a timer  at which to resend the next refresh based upon the 
        // assumption that the request will succeed.  When we receive a 
        // failed response, we will cancel the timer and reschedule
        // a new timer based upon a smaller fraction of the requested 
        // expiration period 
        setRefreshTimer(*state, 
                        TRUE); // Resend with successful timeout
        OsTimer* resendTimer = state->mpRefreshTimer;

        OsSysLog::add(FAC_SIP, PRI_DEBUG,
                      "SipRefreshManager::initiateRefresh refreshTimer just being set.");

        // Mark the refresh state as having an outstanding request
        // and make a copy of the request.  The copy needs to be
        // attached to the state before the send incase the response
        // comes back before we return from the send.
        state->mRequestState = REFRESH_REQUEST_PENDING;
        state->mpLastRequest = new SipMessage(subscribeOrRegisterRequest);

        // Add the state to the container of refresh states
        // No need to lock this refreshMgr earlier as this is a new
        // state and no one can touch it until it is in the list.
        lock();
        mRefreshes.insert(state);
        unlock();
        // NOTE: at this point is is no longer safe to touch the state
        // without locking it again.  Avoid locking this refresh mgr
        // when something can block (e.g. like calling SipUserAgent ::send)

        // Send the request
        // Is the correct?  Should we send the request first and only set
        // a timer if the request succeeds??
        intitialRequestSent = mpUserAgent->send(subscribeOrRegisterRequest);

        // We do not clean up the state even if the send fails.
        // The application must end the refresh as the refresh
        // manager should retry the send if it failed
        if(!intitialRequestSent)
        {
            // Need to lock this refresh mgr and make sure the state
            // still exists.  It could have been deleted while this
            // was unlocked above.
            lock();
            if(stateExists(state))
            {
                // It is now safe to touch the state again

                // Mark the state of the last request as having 
                // failed, so we know to resend when the timer
                // fires
                state->mRequestState = REFRESH_REQUEST_FAILED;

                // The expiration should still be set to zero

                // the initial send failed, cancel the timer and
                // fire the notification.  The handleMessage method
                // will see that the subscription or registration
                // has never succeeded and reshedule the timer for
                // a failure timeout.  We cannot reschedule the
                // timer here as there is a race condition between
                // deleting the timer here (in the applications context)
                // and in handleMessage in this refresh manager's 
                // context.
                // If the timer has not changed assume it is still safe
                // to touch it
                if(state->mpRefreshTimer == resendTimer)
                {
                    stopTimerForFailureReschedule(state->mpRefreshTimer);
                }

                // Do not notify the application that the request failed
                // when it occurs on the first invokation.  The application
                // will know by the return.
            }
            unlock();
        }
    }

    return(intitialRequestSent);
}


UtlBoolean SipRefreshManager::stopRefresh(const char* dialogHandle)
{
    UtlBoolean stateFound = FALSE;
    lock();
    // Find the refresh state
    UtlString dialogHandleString(dialogHandle);
    RefreshDialogState* state = getAnyDialog(dialogHandleString);

    // Remove the state so we can release the lock
    if(state)
    {
        mRefreshes.removeReference(state);
    }
    unlock();

    // If a matching state exists
    if(state)
    {
        // If the subscription or registration has not expired
        // or there is a pending request
        long now = OsDateTime::getSecsSinceEpoch();
        if(state->mExpiration > now || 
           state->mRequestState == REFRESH_REQUEST_PENDING)
        {
            if(state->mpLastRequest)
            {
                // Reset the request with a zero expiration
                setForResend(*state,
                             TRUE); // expire now

                // Don't really need to set this stuff as we are
                // going to delete the state anyway
                state->mRequestState = REFRESH_REQUEST_PENDING;
                state->mPendingStartTime = now;
                state->mExpirationPeriodSeconds = 0;

                mpUserAgent->send(*(state->mpLastRequest));

                // Invoke the refresh state call back to indicate
                // the refresh has been expired
                UtlBoolean stateKeyIsEarlyDialog = SipDialog::isEarlyDialog(*state);
                (state->mpStateCallback)(state->mRequestState,
                                         stateKeyIsEarlyDialog ? state->data() : NULL,
                                         stateKeyIsEarlyDialog ? NULL : state->data(),
                                         state->mpApplicationData,
                                         -1, // responseCode
                                         NULL, // responseText,
                                         0, // zero means expires now
                                         NULL); // response
            }

            // No prior request for some reason
            else
            {
                OsSysLog::add(FAC_SIP, PRI_ERR,
                    "SipRefreshManager::stopRefresh state with NULL mpLastRequest");
            }
        }

        // Stop and delete the refresh timer
        state->mpRefreshTimer->stop();
        deleteTimerAndEvent(state->mpRefreshTimer);

        // Get rid of the dialog
        mpDialogMgr->deleteDialog(*state);

        // Fire and forget
        delete state;
        state = NULL;

        stateFound = TRUE;
    }


    return(stateFound);
}

void SipRefreshManager::stopAllRefreshes()
{
    //  Not sure if it is safe to take the lock on this
    // and keep it while we unsubscribe and unregister
    // everything.  There are some locking issues related
    // to handling incoming messages that might be a
    // problem.
    RefreshDialogState* dialogKey = NULL;
    lock();
    UtlHashMapIterator iterator(mRefreshes);
    while((dialogKey = (RefreshDialogState*) iterator()))
    {
        // Unsubscribe or unregister.
        // This also deletes RefreshDialogState object, so it should not
        // be touched afterwards.
        stopRefresh(*dialogKey);
    }
    unlock();

}

UtlBoolean SipRefreshManager::handleMessage(OsMsg &eventMessage)
{
    int msgType = eventMessage.getMsgType();
    int msgSubType = eventMessage.getMsgSubType();

    // Timer fired
    if(msgType == OsMsg::OS_EVENT &&
       msgSubType == OsEventMsg::NOTIFY)
    {
        intptr_t eventData = 0;
        RefreshDialogState* state = NULL;

        ((OsEventMsg&)eventMessage).getUserData((intptr_t&)state);
        ((OsEventMsg&)eventMessage).getEventData(eventData);

        lock();
        // If the state is not still in the list we cannot
        // touch it. It may have been deleted.
        if(state && stateExists(state))
        {
            // Refresh request failed, need to clean up and
            // schedule a refresh in a short/failed time period
            if(eventData == OS_INTERRUPTED)
            {
                // Clean up the timer and notifier
                deleteTimerAndEvent(state->mpRefreshTimer);
                state->mpRefreshTimer = NULL;

                // Create and set a new timer for the failed time out period
                setRefreshTimer(*state, 
                                FALSE);  // Resend with failure timeout
                OsSysLog::add(FAC_SIP, PRI_DEBUG,
                              "SipRefreshManager::handleMessage refreshTimer just being set for the failed timeout.");
            }

            // Normal timer fire, time to refresh
            else if(eventData != 0 ||
               ((OsTimer*)eventData) == state->mpRefreshTimer)
            {
                // Clean up the timer and notifier
                deleteTimerAndEvent(state->mpRefreshTimer);
                state->mpRefreshTimer = NULL;

                // Legitimate states to reSUBSCRIBE or reREGISTER
                if(state->mRequestState == REFRESH_REQUEST_FAILED || 
                    state->mRequestState == REFRESH_REQUEST_SUCCEEDED)
                {
                    // Create and set a new timer for resending assuming
                    // the resend is successful.  If it fails we will
                    // cancel the timer and set a shorter timeout
                    setRefreshTimer(*state, 
                                    TRUE); // Resend with successful timeout

                    OsSysLog::add(FAC_SIP, PRI_DEBUG,
                                  "SipRefreshManager::handleMessage refreshTimer just being set for the normal timeout.");

                    // reset the message for resend
                    setForResend(*state,
                                 FALSE); // do not expire now

                    // Keep track of when this refresh is sent so we know 
                    // when the new expiration is relative to.
                    state->mPendingStartTime = OsDateTime::getSecsSinceEpoch();

                    // Do not want to keep the lock while we send the
                    // message as it could block.  Presumably it is better
                    // to incure the cost of copying the message????
                    SipMessage tempRequest(*(state->mpLastRequest));
                    
                    UtlString lastRequest;
                    int length;
                    state->mpLastRequest->getBytes(&lastRequest, &length);
                    OsSysLog::add(FAC_SIP, PRI_DEBUG, "SipRefreshManager::handleMessage last request = \n%s",
                                  lastRequest.data());
                      
                    unlock();
                    mpUserAgent->send(tempRequest);
                    // do not need the lock any more, but this gives us
                    // clean locking symmetry.  DO NOT TOUCH state or
                    // any of its members BEYOND this point as it may 
                    // have been deleted
                    lock(); 
                }

                // This should not happen
                else
                {
                    OsSysLog::add(FAC_SIP, PRI_ERR,
                        "SipRefreshManager::handleMessage timer fired for state: %d",
                        state->mRequestState);

                    if(state->mRequestState == REFRESH_REQUEST_PENDING)
                    {
                        // Try again later if it was pending
                        setRefreshTimer(*state, 
                                        FALSE); // Resend with failed timeout
                        OsSysLog::add(FAC_SIP, PRI_DEBUG,
                                      "SipRefreshManager::handleMessage refreshTimer just being resent for the failed timeout.");
                    }
                }
            }

            // Bad do not know what happened
            else
            {
                OsSysLog::add(FAC_SIP, PRI_ERR,
                    "SipRefreshManager::handleMessage timer: %"PRIxPTR" does not match states timer: %p",
                    eventData, state->mpRefreshTimer);
            }
        }
        unlock();
    }

    // SIP message
    else if(msgType == OsMsg::PHONE_APP &&
       msgSubType == SipMessage::NET_SIP_MESSAGE)
    {
        const SipMessage* sipMessage = ((SipMessageEvent&)eventMessage).getMessage();
        int messageType = ((SipMessageEvent&)eventMessage).getMessageStatus();

        // messageType can be:
        //    SipMessageEvent::TRANSPORT_ERROR for requests that do not get sent
        //            but we would have to register with the SipUserAgent to
        //            receive requests.
        //    SipMessageEvent::AUTHENTICATION_RETRY for 401 or 407 responses
        //            that are resent with credentials.  We ge this message so
        //            that we can keep the dialog info. up to date.
        //    SipMessageEvent::APPLICATION normal messages
        // For now we will treat the APPLICATION and AUTHENTICATION_RETRY
        // identically.


        // If this is a SUBSCRIBE or REGISTER response
        UtlString method;
        int cseq;
        if(sipMessage) sipMessage->getCSeqField(&cseq, &method);
        if(sipMessage &&
           sipMessage->isResponse() &&
             (method.compareTo(SIP_SUBSCRIBE_METHOD) == 0 ||
              method.compareTo(SIP_REGISTER_METHOD) == 0))
        {
            UtlString eventField;
            sipMessage->getEventField(&eventField, NULL);
            // We could validate that the event field is
            // set and is the right event type, but mostly
            // we should not care as we know the event type
            // from the subscription.  We can be tolerant of
            // missing or malformed event headers in the
            // NOTIFY request.  The event header is not
            // required in the subscribe response.

            UtlString dialogHandle;
            UtlString earlyDialogHandle;
            sipMessage->getDialogHandle(dialogHandle);
            UtlBoolean foundDialog = 
                mpDialogMgr->dialogExists(dialogHandle);
            UtlBoolean foundEarlyDialog = FALSE;
            UtlBoolean matchesLastLocalTransaction = FALSE;
            if(foundDialog)
            {
                matchesLastLocalTransaction = 
                    mpDialogMgr->isLastLocalTransaction(*sipMessage, 
                                                        dialogHandle);
            }
            else
            {
                foundEarlyDialog = 
                    mpDialogMgr->getEarlyDialogHandleFor(dialogHandle, 
                                                         earlyDialogHandle);

                if(foundEarlyDialog)
                {
                    matchesLastLocalTransaction =
                        mpDialogMgr->isLastLocalTransaction(*sipMessage, 
                                                          earlyDialogHandle);
                }
            }

#ifdef TEST_PRINT
            osPrintf("Looking for refresh state with dialog handle: %s\n",
                   dialogHandle.data());
            UtlString refreshStateDump;
            dumpRefreshStates(refreshStateDump);
            osPrintf("SipRefreshManager::handleMessage state dump:\n%s\n", refreshStateDump.data());
#endif

            lock();
            // Find the refresh state for this response
            RefreshDialogState* state = NULL;
            if(foundDialog && matchesLastLocalTransaction)
            {
                state = (RefreshDialogState*) mRefreshes.find(&dialogHandle);
                // Check if the key has the tags reversed
                if(state == NULL)
                {
                    UtlString reversedDialogHandle;
                    SipDialog::reverseTags(dialogHandle, reversedDialogHandle);
                    state = (RefreshDialogState*) 
                        mRefreshes.find(&reversedDialogHandle);
                }
            }
            else if(foundEarlyDialog && matchesLastLocalTransaction)
            {
                state = (RefreshDialogState*) mRefreshes.remove(&earlyDialogHandle);

                // See if the key has the tags reversed
                if(state == NULL)
                {
                    UtlString reversedEarlyDialogHandle;
                    SipDialog::reverseTags(earlyDialogHandle, reversedEarlyDialogHandle);
                    state = (RefreshDialogState*) 
                        mRefreshes.remove(&reversedEarlyDialogHandle);
                }

                if(state)
                {
#ifdef TEST_PRINT
                    osPrintf("Removed refresh state with dialog handle: %s\n",
                             state->data());
                    osPrintf("Inserting refresh state with dialog handle: %s\n",
                             dialogHandle.data());
#endif
                    // Fix the state handle and put it back in the list
                    *((UtlString*) state) = dialogHandle;
                    mRefreshes.insert(state);
                }
            }

            if(state)
            {

                // Need to check for error responses or 2xx class responses
                int responseCode = sipMessage->getResponseStatusCode();
                UtlString responseText;
                sipMessage->getResponseStatusText(&responseText);

                // Update the expiration members
                int expirationPeriod = 0;
                if(responseCode >= SIP_2XX_CLASS_CODE &&
                   responseCode < SIP_3XX_CLASS_CODE)
                {
                    // Should we tolerate no Expires header in response?
                    // Currently assume that if Expires header is not
                    // set that we got what we asked for
                    if(!getAcceptedExpiration(state, *sipMessage, 
                        expirationPeriod))
                    {
                        expirationPeriod = state->mExpirationPeriodSeconds;
                    }

                    // SUBSCRIBE or REGISTER gave us expiration seconds
                    // from when the request was sent.
                    if(expirationPeriod > 0)
                    {
                        state->mExpiration = state->mPendingStartTime +
                            expirationPeriod;
                    }
                    // UnSUBSCRIBE or unREGISTER
                    else
                    {
                        state->mExpiration = 0;
                    }

                    // The request succeeded
                    state->mRequestState = REFRESH_REQUEST_SUCCEEDED;
                }

                // Provisional response, do nothing
                else if(responseCode < SIP_2XX_CLASS_CODE)
                {
                }

                // There was a resend with credentials for this
                // failed response (should be a 401 or 407 auth.
                // challenge.
                else if(messageType == SipMessageEvent::AUTHENTICATION_RETRY)
                {
                    // Do not stop the timer and do not change the
                    // state from PENDING to FAILED
                }

                // a non-success response code, don't care what
                // type of error.  It is the applications job
                // to care.  If the credentials are wrong, the
                // application needs to fix it.  We will just keep
                // on trying, hoping that the credentials get fixed on 
                //this side or on the server side
                else
                {
                    state->mFailedResponseCode = responseCode;
                    state->mFailedResponseText = responseText;
                    state->mRequestState = REFRESH_REQUEST_FAILED;
                    // Do not change the expiration date, it
                    // is what ever it was before the response was
                    // sent.

                    // Kill the timer and have it fire now and 
                    // reschedule with the error timeout.  We
                    // cannot delete the timer or notifier here 
                    // as there is a race condition where the 
                    // timer might fire before we delete it.  Then
                    // it would be deleted twice (i.e. bad).
                    stopTimerForFailureReschedule(state->mpRefreshTimer);
                }

                //updateState(state, sipMessage);
                mpDialogMgr->updateDialog(*sipMessage);

                // Invoke the callback to let the application
                // know that the state changed
                if(state->mpStateCallback)
                {
                    (state->mpStateCallback)(state->mRequestState,
                                     earlyDialogHandle,
                                     dialogHandle,
                                     state->mpApplicationData,
                                     responseCode, // responseCode
                                     responseText, // responseText,
                                     state->mExpiration, // zero means expires now
                                     sipMessage); // response
                }
            }
            unlock();
        }  // endif SUBSCRIBE or REGISTER response
    } // endif SipMessage event

            return(TRUE);
}


/* ============================ ACCESSORS ================================= */

void SipRefreshManager::refreshState2String(RefreshRequestState state, 
                                            UtlString& stateString)
{
    switch(state)
    {
    case REFRESH_REQUEST_UNKNOWN:
        stateString = "REFRESH_REQUEST_UNKNOWN";
        break;
    case REFRESH_REQUEST_PENDING:
        stateString = "REFRESH_REQUEST_PENDING";
        break;
    case REFRESH_REQUEST_FAILED:
        stateString = "REFRESH_REQUEST_FAILED";
        break;
    case REFRESH_REQUEST_SUCCEEDED:
        stateString = "REFRESH_REQUEST_SUCCEEDED";
        break;

    default:
        {
            stateString = "unknown: ";
            char numBuf[20];
            sprintf(numBuf, "%d", state);
            stateString.append(numBuf);
        }
        break;
    }
}

int SipRefreshManager::dumpRefreshStates(UtlString& dumpString)
{
    int count = 0;
    dumpString.remove(0);
    lock();
    UtlHashMapIterator iterator(mRefreshes);
    RefreshDialogState* state = NULL;
    UtlString oneStateDump;

    while((state = (RefreshDialogState*) iterator()))
    {
        state->toString(oneStateDump);
        dumpString.append(oneStateDump);
        count++;
    }
    unlock();
    return(count);
}


/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */

void SipRefreshManager::lock()
{
    mRefreshMgrMutex.acquire();
}

void SipRefreshManager::unlock()
{
    mRefreshMgrMutex.release();
}

/* //////////////////////////// PRIVATE /////////////////////////////////// */

RefreshDialogState* SipRefreshManager::getAnyDialog(UtlString& messageDialogHandle)
{
    RefreshDialogState* state = (RefreshDialogState*)
        mRefreshes.find(&messageDialogHandle);

    if(state == NULL)
    {
        UtlString reversedHandle;
        SipDialog::reverseTags(messageDialogHandle, reversedHandle);
        state = (RefreshDialogState*)
            mRefreshes.find(&reversedHandle);
    }

    // It did not match
    if(state == NULL)
    {
        // If this is an early dialog handle find out what the 
        // established dialog is      
        UtlString establishedDialogHandle;
        if(mpDialogMgr->getEstablishedDialogHandleFor(messageDialogHandle,
                                                establishedDialogHandle))
        {
            state = (RefreshDialogState*) 
                mRefreshes.find(&establishedDialogHandle);
            if(state == NULL)
            {
                UtlString reversedEstablishedDialogHandle;
                SipDialog::reverseTags(establishedDialogHandle, reversedEstablishedDialogHandle);
                state = (RefreshDialogState*) 
                    mRefreshes.find(&reversedEstablishedDialogHandle);
            }
        }

        // If this is an established dialog, find out what the
        // early dialog handle was and see if we can find it
        else
        {
            UtlString earlyDialogHandle;
            mpDialogMgr->getEarlyDialogHandleFor(messageDialogHandle,
                                           earlyDialogHandle);

            state = (RefreshDialogState*) 
                mRefreshes.find(&earlyDialogHandle);
            if(state == NULL)
            {
                UtlString reversedEarlyDialogHandle;
                SipDialog::reverseTags(earlyDialogHandle, reversedEarlyDialogHandle);
                state = (RefreshDialogState*) 
                    mRefreshes.find(&reversedEarlyDialogHandle);
            }
        }
    }

    return(state);
}

UtlBoolean SipRefreshManager::stateExists(RefreshDialogState* statePtr)
{
    // Assume we already have the lock

    // Does not seem to be a method to test if the reference
    // exists.  If this end up being a performance problem
    // should probably add a method to UtlHashBag
    RefreshDialogState* state = (RefreshDialogState*)
        mRefreshes.removeReference(statePtr);

    if(state)
    {
        mRefreshes.insert(state);
    }

    return(state != NULL);
}

RefreshDialogState* 
    SipRefreshManager::createNewRefreshState(SipMessage& subscribeOrRegisterRequest,
                                              UtlString& messageDialogHandle,
                                              void* applicationData,
                                              const RefreshStateCallback refreshStateCallback,
                                              int& requestedExpiration)
{
    RefreshDialogState* state = new RefreshDialogState();
    *((UtlString*) state) = messageDialogHandle;
    state->mpApplicationData = applicationData;
    state->mpStateCallback = refreshStateCallback;
    if(!getInitialExpiration(subscribeOrRegisterRequest, 
        state->mExpirationPeriodSeconds)) // original expiration
    {
        state->mExpirationPeriodSeconds = mDefaultExpiration;
        subscribeOrRegisterRequest.setExpiresField(mDefaultExpiration);
    }

    requestedExpiration = state->mExpirationPeriodSeconds;

    state->mPendingStartTime = 0;
    state->mExpiration = 0;
    state->mRequestState = REFRESH_REQUEST_UNKNOWN;
    state->mFailedResponseCode = 0;
    state->mFailedResponseText = NULL;
    state->mpRefreshTimer = NULL;  
    state->mpLastRequest = NULL;

    return(state);
}

void SipRefreshManager::setRefreshTimer(RefreshDialogState& state, 
                                        UtlBoolean isSuccessfulReschedule)
{
    // Create and set a new timer for the failed time out period
    int nextResendSeconds = 
        calculateResendTime(state.mExpirationPeriodSeconds,
                                  isSuccessfulReschedule);

    // If a signficant amount of time has passed since the prior
    // request was sent, decrease the error timeout a bit.
    // This is only a problem with the error case as in the
    // successful case we set the timer before sending the
    // request.
    if(!isSuccessfulReschedule)
    {
        long now = OsDateTime::getSecsSinceEpoch();
        if(state.mPendingStartTime > 0 &&
            now - state.mPendingStartTime > 5)
        {
            nextResendSeconds = nextResendSeconds - now + state.mPendingStartTime;
            if(nextResendSeconds < 30)
            {
                nextResendSeconds = 30;
            }
        }
    }

    OsSysLog::add(FAC_SIP, PRI_DEBUG,
                  "SipRefreshManager::setRefreshTimer setting resend timeout in %d seconds\n",
                  nextResendSeconds);

    OsMsgQ* incomingQ = getMessageQueue();
    OsTimer* resendTimer = new OsTimer(incomingQ,
        (intptr_t)&state);
    state.mpRefreshTimer = resendTimer;
    OsTime timerTime(nextResendSeconds, 0);
    resendTimer->oneshotAfter(timerTime);                

}

int SipRefreshManager::calculateResendTime(int requestedExpiration, 
                        UtlBoolean isSuccessfulResend)
{
    int expiration;
    if(isSuccessfulResend)
    {
        expiration = (int)(0.55 * requestedExpiration);
    }
    else
    {
        expiration = (int)(0.1 * requestedExpiration);
    }

    // Clamp it to a minimum of a transaction timeout
    int minRefresh = (mpUserAgent->getSipStateTransactionTimeout())/1000;
    if(expiration < minRefresh)
    {
        expiration = minRefresh;
    }

    return(expiration);
}

void SipRefreshManager::stopTimerForFailureReschedule(OsTimer* resendTimer)
{
    if(resendTimer)
    {
        resendTimer->stop();
        OsQueuedEvent* queuedEvent = 
            (OsQueuedEvent*) resendTimer->getNotifier();

        // If the queued event exists fire it now with an error status
        // to indicate that it should be resheduled with the (shorter)
        // error timeout.  Normally the timer is scheduled with the
        // timeout assuming that the reSUBSCRIBE or reREGISTER will
        // succeed.
        if(queuedEvent)
        {
            // Effectively make the timer fire now
            queuedEvent->signal(OS_INTERRUPTED);  // CANCELED
        }
    }
}

void SipRefreshManager::deleteTimerAndEvent(OsTimer* timer)
{
    if(timer)
    {
        delete timer;
        timer = NULL;
    }
}

void SipRefreshManager::setForResend(RefreshDialogState& state, 
                                     UtlBoolean expireNow)
{
    if(state.mpLastRequest)
    {
        UtlString lastRequest;
        int length;
        state.mpLastRequest->getBytes(&lastRequest, &length);
        OsSysLog::add(FAC_SIP, PRI_DEBUG,
                      "SipRefreshManager::setForResend last request = \n%s",
                      lastRequest.data());
       
        // Remove old vias
        state.mpLastRequest->removeLastVia();

        // Remove old routes
        UtlString route;
        while(state.mpLastRequest->removeRouteUri(0, &route))
        {
        }

        // Remove any credentials
        while(state.mpLastRequest->removeHeader(HTTP_AUTHORIZATION_FIELD, 0))
        {
        }
        while(state.mpLastRequest->removeHeader(HTTP_PROXY_AUTHORIZATION_FIELD, 0))
        {
        }

        // Remove transport state info
        state.mpLastRequest->resetTransport();

        // Set the dialog info and cseq
        mpDialogMgr->setNextLocalTransactionInfo(*(state.mpLastRequest));

        // Set the expiration
        if (expireNow)
        {
           state.mpLastRequest->setExpiresField(0);
        }
        else
        {
           state.mpLastRequest->setDateField();
        }
    }
}


UtlBoolean SipRefreshManager::getInitialExpiration(const SipMessage& sipRequest, 
                                            int& expirationPeriod)
{
    UtlString method;
    UtlBoolean foundExpiration = FALSE;
    sipRequest.getRequestMethod(&method);

    if(method.compareTo(SIP_REGISTER_METHOD) == 0)
    {
        // Register could have it in the Contact header
        UtlString requestContactValue;
        if(sipRequest.getContactEntry(0 , &requestContactValue))
        {
            // Get the expires parameter for the contact if it exists
            Url contactUri(requestContactValue);
            UtlString contactExpiresParameter;
            if(contactUri.getFieldParameter(SIP_EXPIRES_FIELD, 
                    contactExpiresParameter) &&
               !contactExpiresParameter.isNull())
            {
                foundExpiration = TRUE;

                // Convert to int
                expirationPeriod = atoi(contactExpiresParameter);
            }
        }
    }

    if(!foundExpiration)
    {
        // Not sure if we care if this is a request or response
        foundExpiration = sipRequest.getExpiresField(&expirationPeriod);
    }

    return(foundExpiration);
}

UtlBoolean SipRefreshManager::getAcceptedExpiration(RefreshDialogState* state,
                                                    const SipMessage& sipResponse, 
                                                    int& expirationPeriod)
{
    UtlString method;
    UtlBoolean foundExpiration = FALSE;
    int cseq;
    sipResponse.getCSeqField(&cseq, &method);

    if(method.compareTo(SIP_REGISTER_METHOD) == 0)
    {

        // Get the presumably first contact in the REGISTER request
        // so that we can find the same contact in the response and
        // find out what the expiration is
        UtlString requestContact;
        Url requestContactUri;
        if(state && state->mpLastRequest &&
           state->mpLastRequest->getContactEntry(0, &requestContact))
        {
           requestContactUri = requestContact;
        }

        // Register could have it in the Contact header
        UtlString responseContactValue;
        int contactIndex = 0;
        while(sipResponse.getContactEntry(contactIndex , &responseContactValue))
        {
            // Get the expires parameter for the contact if it exists
            Url contactUri(responseContactValue);

            if(requestContactUri.isUserHostPortEqual(contactUri))
            {
                UtlString contactExpiresParameter;
                if(contactUri.getFieldParameter(SIP_EXPIRES_FIELD, 
                        contactExpiresParameter) &&
                   !contactExpiresParameter.isNull())
                {
                    foundExpiration = TRUE;

                    // Convert to int
                    expirationPeriod = atoi(contactExpiresParameter);
                }
            }
            contactIndex++;
        }
    }

    if(!foundExpiration)
    {
        // Not sure if we care if this is a request or response
        foundExpiration = sipResponse.getExpiresField(&expirationPeriod);
    }

    return(foundExpiration);
}

/* ============================ FUNCTIONS ================================= */

