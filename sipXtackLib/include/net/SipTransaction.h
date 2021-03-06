//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _SipTransaction_h_
#define _SipTransaction_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES


#include <os/OsDefs.h>
#include <os/OsSocket.h>
#include <os/OsMsgQ.h>
#include <net/Url.h>
#include <net/SipSrvLookup.h>
#include <utl/UtlSList.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class SipMessage;
class SipUserAgent;
class OsEvent;
class OsTimer;
class SIPX_TRANSPORT_DATA;
class SipTransactionList;

//#include "tapi/sipXtapiInternal.h"

/** SipTransaction correlates requests and responses.
 * 
 * CallId  + 's' or 'c' (for server or client) is used as
 * the key for the hash (i.e. stored as the string/data in
 * the parent UtlString.
 */
class SipTransaction : public UtlString {
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

    static int smTransactionNum;
    static UtlString smBranchIdBase;


    /*** States of a transaction.
     * @note
     *   See RFC 3261 for a defininition and description
     *   of these transaction states and when the transitions occur.
     * @endnote
     */
    enum transactionStates {
        TRANSACTION_UNKNOWN = 0,       ///< not yet set
        TRANSACTION_LOCALLY_INIITATED, ///< No messages sent (usually client)
        TRANSACTION_CALLING,           ///< Request sent
        TRANSACTION_PROCEEDING,        ///< Provisional response received
        TRANSACTION_COMPLETE,          ///< Final response received
        TRANSACTION_CONFIRMED,         ///< ACK recieved for 300-699 response classes
        TRANSACTION_TERMINATED
    };

    /// The relationship of a message to a transaction
    enum messageRelationship {
        MESSAGE_UNKNOWN,          ///< Relationship not yet determined, or error
        MESSAGE_UNRELATED,        ///< A with different Call-Id, To or From
        MESSAGE_SAME_SESSION,     ///< But not part of this TX or related branches
        MESSAGE_DIFFERENT_BRANCH, ///< Same Call-Id, to, from, cseq but different TX
        MESSAGE_REQUEST,          ///< The request to this TX
        MESSAGE_PROVISIONAL,      ///< A provision response to this TX
        MESSAGE_FINAL,            ///< The first final response to this TX
        MESSAGE_NEW_FINAL,        ///< A different final response for this TX
        MESSAGE_CANCEL,           ///< A cancel for this TX
        MESSAGE_CANCEL_RESPONSE,
        MESSAGE_ACK,              ///< An ACK for this non-2xx TX
        MESSAGE_2XX_ACK,          ///< An ACK assocated with this TX (but considered a different TX)
        MESSAGE_DUPLICATE         ///< A duplicate message for this TX
    };

/* ============================ CREATORS ================================== */

    SipTransaction(SipMessage* request = NULL,
                   UtlBoolean isOutgoing = TRUE,
                   UtlBoolean userAgentTransaction = TRUE);
    //:Default constructor
    // When this is an out going request, this is a client
    // transaction.  The via header field MUST be added before
    // constructing this transaction as this sets the branch ID.
    //! param: userAgentTransaction - user agent (vs proxy) client or
    //         server



    virtual
    ~SipTransaction();
    //:Destructor

/* ============================ MANIPULATORS ============================== */

    void getNewBranchId(SipMessage& request,
                        UtlString& branchId);

    UtlBoolean handleOutgoing(SipMessage& outgoingMessage,
                             SipUserAgent& userAgent,
                             SipTransactionList& transactionList,
                             enum messageRelationship relationship,
                             SIPX_TRANSPORT_DATA* pTransport = NULL);

    void handleResendEvent(const SipMessage& outgoingMessage,
                            SipUserAgent& userAgent,
                            enum messageRelationship relationship,
                            SipTransactionList& transactionList,
                            int& nextTimeout,
                            SipMessage*& delayedDispatchedMessage,
                            SIPX_TRANSPORT_DATA* pTransport);

    void handleExpiresEvent(const SipMessage& outgoingMessage,
                            SipUserAgent& userAgent,
                            enum messageRelationship relationship,
                            SipTransactionList& transactionList,
                            int& nextTimeout,
                            SipMessage*& delayedDispatchedMessage,
                            SIPX_TRANSPORT_DATA* pTransport);

    UtlBoolean handleIncoming(SipMessage& incomingMessage,
                             SipUserAgent& userAgent,
                             enum messageRelationship relationship,
                             SipTransactionList& transactionList,
                             SipMessage*& delayedDispatchedMessage,
                             SIPX_TRANSPORT_DATA* pTransport);

    void removeTimer(OsTimer* timer);

    void stopTimers() ;
    void startTimers();
    void deleteTimers();

/* ============================ Deprecated ============================== */

    void linkChild(SipTransaction& child);

    void toString(UtlString& dumpString,
                  UtlBoolean dumpMessagesAlso);
    //: Serialize the contents of this

    void dumpTransactionTree(UtlString& dumpstring, 
                             UtlBoolean dumpMessagesAlso);
    //: Serialize the contents of all the transactions in this tree
    // The parent is found first and then all children are serialized
    // recursively

    void dumpChildren(UtlString& dumpstring, 
                      UtlBoolean dumpMessagesAlso);
    //: Serialize the contents of all the child transactions to this transaction
    // All children are serialized recursively


/* ============================ ACCESSORS ================================= */

    static void getStateString(enum transactionStates state,
                               UtlString& stateString);

    static void getRelationshipString(enum messageRelationship relationship,
                                      UtlString& relationshipString);

    static void buildHash(const SipMessage& message, 
                          UtlBoolean isOutgoing,
                          UtlString& hash);

    SipTransaction* getTopMostParent() const;

    void getCallId(UtlString& callId) const;

    enum transactionStates getState() const;

    long getStartTime() const;

    long getTimeStamp() const;

    void touch();
    void touchBelow(int newDate);

    SipMessage* getRequest();

    SipMessage* getLastProvisionalResponse();

    SipMessage* getLastFinalResponse();

    void cancel(SipUserAgent& userAgent,
                SipTransactionList& transactionList);
    //: cancel any outstanding client transactions (recursively on children)

    void markBusy();

    void markAvailable();

    void notifyWhenAvailable(OsEvent* availableEvent);
    //: The given event is signaled when this transaction is not busy

    void signalNextAvailable();

    void signalAllAvailable();

/* ============================ INQUIRY =================================== */

    UtlBoolean isServerTransaction() const;
    //: Inquire if this transaction is a server as opposed to a client transaction

    //! Inquiry as to whether this transaction is a recursed DNS SRV child
    UtlBoolean isDnsSrvChild() const;

    UtlBoolean isUaTransaction() const;
    //: Inquire if transaction is UA based or proxy
    // Note this is different than server vs client transaction

    UtlBoolean isChildSerial();
    //: Inquire as to whether child transaction will be serial or all parallel searched
    // If all immediate child transactions have the same 
    // Q value FALSE is returned

    UtlBoolean isEarlyDialogWithMedia();
    //: Tests to see if this is an existing early dialog with early media
    // If transaction has not yet been completed and there was early media
    // (determined by the presence of SDP in a provisional response

    UtlBoolean isChildEarlyDialogWithMedia();
    //: Are any of the children in an early dialog with media

    UtlBoolean isMethod(const char* methodToMatch) const;
    //: see if this tranaction is of the given method type

    enum messageRelationship whatRelation(const SipMessage& message,
                                          UtlBoolean isOutgoing) const;
    //: Check if the given message is part of this transaction

    UtlBoolean isBusy();
    //: is this transaction being used (e.g. locked)

    //UtlBoolean isDuplicateMessage(SipMessage& message,
    //                             UtlBoolean checkIfTransactionMatches = TRUE);
    //: Check to see if this request or response has already been received by this transaction

    UtlBoolean isUriChild(Url& uri);
    // Does this URI already exist as an immediate child to this transaction
    // Search through each of the children and see if the child
    // transaction's URI matches.

    UtlBoolean isUriRecursed(Url& uri);
    // Has this URI been recursed anywhere in this transaction tree already
    // Start looking at the parent

    UtlBoolean isUriRecursedChildren(UtlString& uriString);
    // Has this URI been recursed anywhere at or below in this transaction tree already
    // Look at or below the current transaction in the transaction tree

    void setTransport(SIPX_TRANSPORT_DATA* pTransport) { mpTransport = pTransport; }
    // Mutator for mpTransport.

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
    void handleChildTimeoutEvent(SipTransaction& child,
                                 const SipMessage& outgoingMessage,
                                 SipUserAgent& userAgent,
                                 enum messageRelationship relationship,
                                 SipTransactionList& transactionList,
                                 int& nextTimeout,
                                 SipMessage*& delayedDispatchedMessage,
                                 SIPX_TRANSPORT_DATA* pTransport);
    //: tells the parent transaction the result of the timeout event

    UtlBoolean handleChildIncoming(//SipTransaction& child,
                                  SipMessage& incomingMessage,
                                  SipUserAgent& userAgent,
                                  enum messageRelationship relationship,
                                  SipTransactionList& transactionList,
                                  UtlBoolean childSaysShouldDispatch,
                                  SipMessage*& delayedDispatchedMessage,
                                  SIPX_TRANSPORT_DATA* pTransport);
    //: Tells the parent transaction the result of the incoming message
    //! returns: TRUE/FALSE as to whether the message should be dispatched to applications

    UtlBoolean startSequentialSearch(SipUserAgent& userAgent,
                                    SipTransactionList& transactionList,
                                    SIPX_TRANSPORT_DATA* pTransport);
    //: Checks to see if a final response can be sent or if sequential search should be started

    UtlBoolean recurseChildren(SipUserAgent& userAgent,
                              SipTransactionList& transactionList);
    //: Starts search on any immediate children of the highest unpursued Q value

    UtlBoolean recurseDnsSrvChildren(SipUserAgent& userAgent,
                              SipTransactionList& transactionList,
                              SIPX_TRANSPORT_DATA* pTransport);
    //: Starts search on any immediate DNS SRV children of the highest unpursued Q value

    UtlBoolean findBestResponse(SipMessage& bestResponse);
    // Finds the best final response to return the the server transaction

    enum messageRelationship addResponse(SipMessage*& response,
                                         UtlBoolean isOutGoing,
                                         enum messageRelationship relationship = MESSAGE_UNKNOWN);
    //: Adds the provisional or final response to the transaction

    void cancelChildren(SipUserAgent& userAgent,
                        SipTransactionList& transactionList);
    //: Cancels children transactions on a server transaction

    void doMarkBusy(int markValue);

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
    SipTransaction(const SipTransaction& rSipTransaction);
    //:Copy constructor (disabled)
    SipTransaction& operator=(const SipTransaction& rhs);
    //:Assignment operator (disabled)

    UtlBoolean doResend(SipMessage& resendMessage,
                       SipUserAgent& userAgent,
                       int& nextTimeoutMs,
                       SIPX_TRANSPORT_DATA* pTransport);

    UtlBoolean doFirstSend(SipMessage& message,
                          enum messageRelationship relationship,
                          SipUserAgent& userAgent,
                          UtlString& toAddress,
                          int& port,
                          OsSocket::IpProtocolSocketType& toProtocol,
                          SIPX_TRANSPORT_DATA* pTransport);

    void prepareRequestForSend(SipMessage& request,
                               SipUserAgent& userAgent,
                               UtlBoolean& addressRequiresDnsSrvLookup,
                               UtlString& toAddress,
                               int& port,
                               OsSocket::IpProtocolSocketType& toProtocol);

    // CallId  + 's' or 'c' (for server or client) is used as
    // the key for the hash (i.e. stored as the string/data in
    // the parent UtlString
    UtlString mCallId;
    UtlString mBranchId;
    UtlString mRequestUri;
    Url mFromField;
    Url mToField;
    UtlString mRequestMethod;
    int mCseq;
    UtlBoolean mIsServerTransaction; ///< TRUE = server, FALSE = client
    UtlBoolean mIsUaTransaction;     ///< UA or proxy transaction

    // Address and transport that have been established for this transaction.
    UtlString mSendToAddress;
    int mSendToPort;
    OsSocket::IpProtocolSocketType mSendToProtocol;

    server_t* mpDnsSrvRecords;
    SipMessage* mpRequest;
    SipMessage* mpLastProvisionalResponse;
    SipMessage* mpLastFinalResponse;
    SipMessage* mpAck;
    SipMessage* mpCancel;
    SipMessage* mpCancelResponse;
    SipTransaction* mpParentTransaction;
    UtlSList mChildTransactions;
    long mTransactionCreateTime;         ///< When this thing was created
    long mTransactionStartTime;          /**<  When the request was sent/received
                                          * i.e. went to TRANSACTION_CALLING state */
    long mTimeStamp;                     ///< When this was last used
    enum transactionStates mTransactionState;
    UtlBoolean mDispatchedFinalResponse; ///< For UA recursion
    UtlBoolean mProvisionalSdp;          ///< early media
    UtlSList mTimers;                    /**< A list of all outstanding timers
                                          *   started by this transaction. */

    // Recursion members
    UtlBoolean mIsCanceled;
    UtlBoolean mIsRecursing;   ///< TRUE if any braches have not be pursued
    UtlBoolean mIsDnsSrvChild; ///< This CT pursues one of the SRV records of the parent CT
    double mQvalue;            ///< Recurse order.  equal values are recursed in parallel
    int mExpires;              ///< Maximum time (seconds) to wait for a final outcome
    UtlBoolean mIsBusy;
    UtlString mBusyTaskName;
    UtlSList* mWaitingList;    /**< Events waiting until this is available
                                * Note only a parent tx should have a waiting list */
    SIPX_TRANSPORT_DATA* mpTransport; 
    //: An optional external transport mechanism pointer.  If this value is non-null, the
    //: transaction will use the SipUserAgent::sendCustom method, instead of sendUdp, sendTcp, etc.

};

/* ============================ INLINE METHODS ============================ */

#endif // _SipTransaction_h_
