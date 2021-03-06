//
// Copyright (C) 2005-2008 SIPez LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// Copyright (C) 2004-2008 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////
// Author: Dan Petrie (dpetrie AT SIPez DOT com)


#ifndef _HttpBody_h_
#define _HttpBody_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <utl/UtlString.h>
#include <utl/UtlDList.h>

// DEFINES
#define CONTENT_TYPE_TEXT_PLAIN "text/plain"
#define CONTENT_TYPE_TEXT_HTML "text/html"
#define CONTENT_SMIME_PKCS7 "application/pkcs7-mime"
#define CONTENT_TYPE_PIDF "application/pidf+xml"
#define CONTENT_TYPE_MULTIPART "multipart/"
#define CONTENT_TYPE_MULTIPART_RELATED "multipart/related"
#define CONTENT_TYPE_CONTENT_INDIRECTION "message/external-body"
#define DIALOG_EVENT_CONTENT_TYPE "application/dialog-info+xml"

#define MULTIPART_BOUNDARY_PARAMETER "boundary"

#define MAX_HTTP_BODY_PARTS 20

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class MimeBodyPart;

//! class to contain an HTTP body
/*! This is the base class and container for all HTTP (SIP, etc.)
 * message bodies.  This includes multipart MIME bodies, single
 * part MIME and specific MIME types.  The HttpBody is essentially
 * a container for a blob.
 */
class HttpBody : public UtlString
{
   friend class HttpBodyMultipart;

/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

    enum BodyClassTypes
    {
        UNKNOWN_BODY_CLASS = 0,
        HTTP_BODY_CLASS,
        SMIME_BODY_CLASS,
        SDP_BODY_CLASS,
        PIDF_BODY_CLASS,
        DIALOG_EVENT_BODY_CLASS
    };

/* ============================ CREATORS ================================== */

   HttpBody(const char* bytes = NULL, 
            int length = -1,
            const char* contentType = NULL);
   //: Construct an HttpBody from a bunch of bytes

   HttpBody(const char* contentType);
   //: Construct a multipart HttpBody with zero parts.
   // contentType should have no "boundary" parameter.

   HttpBody(const HttpBody& rHttpBody);
     //:Copy constructor

   virtual
   ~HttpBody();
     //:Destructor

/* ============================ MANIPULATORS ============================== */

   HttpBody& operator=(const HttpBody& rhs);
   //:Assignment operator

   //! Copy the source HttpBody using the correct derived class constructor
   static HttpBody* copyBody(const HttpBody& sourceBody);

   //! Pseudo body factory
   static HttpBody* createBody(const char* bodyBytes,
                               int bodyBytesLength,
                               const char* contentType,
                               const char* contentEncoding);

   //! Append a multipart body part to an existing multiparty body.
   void appendBodyPart(const HttpBody& body,
                       const UtlDList& parameters);

/* ============================ ACCESSORS ================================= */

   virtual int getLength() const;

   // Note: for convenience, bytes is null terminated
   // However depending upon the content type, the body may
   // contain more than one null character.
   // *bytes != NULL, even if *length == 0.
   virtual void getBytes(const char** bytes, int* length) const;
   virtual void getBytes(UtlString* bytes, int* length) const;
   virtual const char* getBytes() const;

   UtlBoolean getMultipartBytes(int partIndex, 
                                const char** bytes,
                                int* length,
                                int* start) const;

   const MimeBodyPart* getMultipart(int partIndex) const;

   int getMultipartCount() const;

   BodyClassTypes getClassType() const;

   //! Get the multipart boundary string.
   // Valid while HttpBody exists and is not modified.
   const char* getMultipartBoundary() const;

   //! Get the content type string.
   // Valid while HttpBody exists and is not modified.
   const char* getContentType() const;

/* ============================ INQUIRY =================================== */

   UtlBoolean isMultipart() const;

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

   int bodyLength;
   UtlString mBody;
   UtlString  mMultipartBoundary;
   int mBodyPartCount;
   MimeBodyPart* mpBodyParts[MAX_HTTP_BODY_PARTS];
   BodyClassTypes mClassType;

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

   // The counter for generating boundary values.
   static unsigned boundaryCounter;
   // Generate the next boundary value.
   static void nextBoundary(UtlString& boundary);
};


// Carrier class to provide a constructor that would otherwise conflict
// with an existing constructor for HttpBody.
class HttpBodyMultipart : public HttpBody
{
  public:

   HttpBodyMultipart(const char* contentType);
   //: Construct a multipart HttpBody with zero parts.
   // contentType should have no "boundary" parameter.
};


/* ============================ INLINE METHODS ============================ */

#endif  // _HttpBody_h_
