//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _OsConnectionSocket_h_
#define _OsConnectionSocket_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <os/OsSocket.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class UtlSList;

//: Implements TCP version of OsSocket
// This class provides the implementation of the TCP datagram 
// based socket class which may be instantiated.
class OsConnectionSocket : public OsSocket
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

   OsConnectionSocket(int remoteHostPort,
                      const char* remoteHostName,
                      UtlBoolean makeBlocking = TRUE,
                      const char* localIp = NULL,
                      const bool bConnect = true);

   OsConnectionSocket(int connectedSocketDescriptor);

   OsConnectionSocket(const char* localIp,
                      int connectedSocketDescriptor);

  virtual
   ~OsConnectionSocket();
     //:Destructor

/* ============================ MANIPULATORS ============================== */

   virtual int connect();
   //: Sets up the initial connection with the server
    
   virtual UtlBoolean reconnect();
   //: Sets up the connection again, assuming the connection failed

   virtual int read(char* buffer, int bufferLength);
   //:Blocking read from the socket
   // Read bytes into the buffer from the socket up to a maximum of 
   // bufferLength bytes.  This method will block until there is
   // something to read from the socket.
   //!param: buffer - Place to put bytes read from the socket.
   //!param: bufferLength - The maximum number of bytes buffer will hold.
   //!returns: The number of bytes actually read.

   virtual int read(char* buffer, int bufferLength, 
                    UtlString* ipAddress, int* port);
   //:Blocking read from the socket
   // Read bytes into the buffer from the socket up to a maximum of 
   // bufferLength bytes.  This method will block until there is
   // something to read from the socket.
   //!param: buffer - Place to put bytes read from the socket.
   //!param: bufferLength - The maximum number of bytes buffer will hold.
   //!param: ipAddress - The address of the socket that sent the bytes read.
   //!param: port - The port of the socket that sent the bytes read.
   //!returns: The number of bytes actually read.

   virtual int read(char* buffer, int bufferLength, long waitMilliseconds);
   //: Non-blocking or limited blocking read from socket
   // Same as blocking version except that this read will block
   // for no more than the specified length of time.
   //!param: waitMilliseconds - The maximum number of milliseconds to block. This may be set to zero in which case it does not block.

/* ============================ ACCESSORS ================================= */
   virtual OsSocket::IpProtocolSocketType getIpProtocol() const;
   //: Returns the protocol type of this socket


/* ============================ INQUIRY =================================== */

   /// Is this connection encrypted using TLS/SSL?
   virtual bool isEncrypted() const;
   
   /// Get any authenticated peer host names.
   virtual bool peerIdentity( UtlSList* altNames = NULL /**< UtlStrings for verfied subjectAltNames
                                                         *   are added to this
                                                         *   @note caller must free them.
                                                         */
                             ,UtlString* commonName = NULL ///< the Subject name is returned here
                             ) const;
   /**<
    * Usually, the names in the altNames will be easier to parse and use than commonName
    * Either or both of altNames or commonName may be NULL, in which case no names are returned;
    * the return value still indicates the trust relationship with the peer certificate.
    * @returns
    * - true if the connection is TLS/SSL and the peer has presented
    *        a certificate signed by a trusted certificate authority
    * - false if not
    */

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
    bool initialize(const char* serverName, int serverPort, UtlBoolean blockingConnect);

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

   struct sockaddr_in serverSockAddr;
   
   OsConnectionSocket(const OsConnectionSocket& rOsConnectionSocket);
     //:Disable copy constructor

   OsConnectionSocket();
     //:Disable default constructor

   OsConnectionSocket& operator=(const OsConnectionSocket& rhs);
     //:Assignment operator

};

/* ============================ INLINE METHODS ============================ */

#endif  // _OsConnectionSocket_h_

