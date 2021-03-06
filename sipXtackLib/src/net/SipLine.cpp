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

#if defined(_VXWORKS)
#   include <taskLib.h>
#   include <netinet/in.h>
#endif

#include <utl/UtlHashBagIterator.h>
#include <net/SipLine.h>
#include <net/Url.h>
#include <net/SipLineCredentials.h>
#include <os/OsConfigDb.h>
#include <os/OsRWMutex.h>
#include <os/OsReadLock.h>
#include <os/OsWriteLock.h>
#include <os/OsDateTime.h>
#include <os/OsSysLog.h>
#include <net/NetMd5Codec.h>


// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Copy constructor
SipLine::SipLine(const SipLine& rSipLine)
{
   mIsVisible = rSipLine.mIsVisible ;
   mIdentity = rSipLine.mIdentity ;
   mUserEnteredUrl = rSipLine.mUserEnteredUrl;
   mUser = rSipLine.mUser ;
   mCurrentState = rSipLine.mCurrentState ;
   mIsAutoEnabled = rSipLine.mIsAutoEnabled ;
   mIsUsingCallHandling = rSipLine.mIsUsingCallHandling;
   mCanonicalUrl = rSipLine.mCanonicalUrl;
   mLineId = rSipLine.mLineId;
   mPreferredContactUri = rSipLine.mPreferredContactUri ;
   meContactType = rSipLine.meContactType;
   
   copyCredentials(rSipLine);
   
}

SipLine& SipLine::operator=(const SipLine& rSipLine)
{
   if (this == &rSipLine)            // handle the assignment to self case
      return *this;
   else
   {
      mIsVisible = rSipLine.mIsVisible ;
      mIdentity = rSipLine.mIdentity ;
      mUserEnteredUrl = rSipLine.mUserEnteredUrl;
      mCanonicalUrl = rSipLine.mCanonicalUrl;
      mUser = rSipLine.mUser ;
      mCurrentState = rSipLine.mCurrentState ;
      mIsAutoEnabled = rSipLine.mIsAutoEnabled ;
      mIsUsingCallHandling = rSipLine.mIsUsingCallHandling;
      mLineId = rSipLine.mLineId;
      mPreferredContactUri = rSipLine.mPreferredContactUri ;
      meContactType = rSipLine.meContactType;
      copyCredentials(rSipLine);
   }
   return *this;
}
//deep copy of credentials
void SipLine::copyCredentials(const SipLine &rSipLine)
{
   UtlString Realm;
   UtlString UserID;
   UtlString Type;
   UtlString Password;

   if(!mCredentials.isEmpty())
   {
      mCredentials.destroyAll();
   }
   if (! rSipLine.mCredentials.isEmpty())
   {
        UtlHashBagIterator observerIterator(const_cast<UtlHashBag&> (rSipLine.mCredentials));
        SipLineCredentials* credential = NULL;
        do
        {
            credential = (SipLineCredentials*) observerIterator();
            if ( credential)
            {
                credential->getRealm(&Realm);
                credential->getUserId(&UserID);
                credential->getType(&Type);
                credential->getPasswordToken(&Password);
                addCredentials(Realm , UserID , Password , Type);
            }
        }
        while(credential != NULL) ;
   }
}

SipLine::SipLine(Url userEnteredUrl,
                 Url identityUri,
                 UtlString user,
                 UtlBoolean visible,
                 int state,
                 UtlBoolean isAutoEnabled,
                 UtlBoolean useCallHandling)
{
   if (user.isNull())
   {
      mUser = userEnteredUrl.toString();
   }
   else
   {
      mUser = user;
   }

   mIsVisible = visible;
   mCurrentState = state;
   mIsAutoEnabled = isAutoEnabled;
   mIsUsingCallHandling = useCallHandling;
   mUserEnteredUrl = userEnteredUrl;
   meContactType = LINE_CONTACT_NAT_MAPPED ; 
   if (identityUri.toString().isNull())
   {
      //then get uri from user entered url ...uri is complete in it
      UtlString uri;
      mUserEnteredUrl.getUri(uri);
      mIdentity = Url(uri);
   }
   else
   {
      mIdentity = identityUri;
   }

   //construct a complete url from identity and userEntered Url.
   mCanonicalUrl = mUserEnteredUrl;
   UtlString address;
   mUserEnteredUrl.getHostAddress(address);
   if (address.isNull())
   {
      UtlString identityHost;
      mIdentity.getHostAddress(identityHost);
      int identityPort = mIdentity.getHostPort();
      mCanonicalUrl.setHostAddress(identityHost);
      mCanonicalUrl.setHostPort(identityPort);
   }
   // create new Line Id for this line
   generateLineID(mLineId);   
}

SipLine::~SipLine()
{
    mCredentials.destroyAll();
}

UtlString& SipLine::getLineId()
{
   return mLineId;
}


UtlBoolean SipLine::isDeviceLine()
{
    return (getUser().compareTo("Device", UtlString::ignoreCase) == 0) ;
}


int SipLine::getState()
{
    return mCurrentState;
}

void SipLine::setState(int state)
{
    mCurrentState = state;
}

UtlString& SipLine::getUser()
{
    return mUser;
}

void SipLine::setUser(const UtlString user)
{
   mUser.remove(0);
   mUser.append(user);
}

void SipLine::getIdentityAndUrl(Url &identity, Url &userEnteredUrl)
{
   identity = mIdentity;
   userEnteredUrl = mUserEnteredUrl;
}

void SipLine::setIdentityAndUrl(Url identity, Url userEnteredUrl)
{
   UtlString identityHost;
   UtlString address;
   int identityPort;

   mIdentity = identity;
   mLineId.remove(0);
   generateLineID(mLineId);
   mUserEnteredUrl = userEnteredUrl;
   //construct a complete url from identity and userEntered Url.
   mCanonicalUrl = mUserEnteredUrl;
   
   mUserEnteredUrl.getHostAddress(address);
   if (address.isNull())
   {     
      mIdentity.getHostAddress(identityHost);
      identityPort = mIdentity.getHostPort();
      mCanonicalUrl.setHostAddress(identityHost);
      mCanonicalUrl.setHostPort(identityPort);
   }
}


Url& SipLine::getUserEnteredUrl()
{
    return mUserEnteredUrl;
}
Url& SipLine::getIdentity()
{
    return mIdentity;
}

Url& SipLine::getCanonicalUrl()
{
    return mCanonicalUrl;
}

void SipLine::setAutoEnableStatus( UtlBoolean isAutoEnabled)
{
    mIsAutoEnabled = isAutoEnabled;
}

UtlBoolean SipLine::getAutoEnableStatus()
{
    return mIsAutoEnabled ;
}

void SipLine::setVisibility(UtlBoolean isEnable)
{
    mIsVisible = isEnable;
}

UtlBoolean SipLine::getVisibility()
{
    return mIsVisible;
}
void SipLine::setCallHandling(UtlBoolean useCallHandling)
{
    mIsUsingCallHandling = useCallHandling;
}

UtlBoolean SipLine::getCallHandling()
{
    return mIsUsingCallHandling;
}

UtlBoolean SipLine::addCredentials( const UtlString& strRealm,
                             const UtlString& strUserID,
                             const UtlString& strPasswd,
                             const UtlString& Type)
{
   UtlBoolean isAdded = FALSE;

   if(!IsDuplicateRealm(strRealm, Type))
   {
      SipLineCredentials* credential = new SipLineCredentials(strRealm , strUserID, strPasswd, Type);
      mCredentials.insert((UtlString*)credential);
      isAdded = TRUE;
#ifdef TEST_PRINT
      OsSysLog::add(FAC_AUTH, PRI_DEBUG,
          "SipLine::addCredentials(realm=%s, userId=%s, password=%s, type=%s) number of credentials: %d for line: %s",
          strRealm.data(), strUserID.data(), strPasswd.isNull() ? "" : "******", 
          Type.data(), mCredentials.entries(), mIdentity.toString().data());
#endif
   }
   else
   {
      OsSysLog::add(FAC_AUTH, PRI_DEBUG,
          "SipLine::addCredentials(realm=%s, userId=%s, ...) not added as its a duplicate",
          strRealm.data(), strUserID.data());
   }
   return isAdded;
}

int SipLine::GetNumOfCredentials()
{
   return mCredentials.entries();

}

UtlBoolean SipLine::getCredentials(const UtlString& type /*[in]*/,
                             const UtlString& realm /*[in]*/,
                             UtlString* userID /*[out]*/,
                             UtlString* MD5_token /*[out]*/)
{
   UtlBoolean credentialsFound = FALSE;
   UtlString matchRealm(realm);
   UtlString emptyRealm(NULL);
   UtlString userPassword;
   *MD5_token = "";

#ifdef TEST_PRINT
   OsSysLog::add(FAC_AUTH, PRI_DEBUG, "SipLine::getCredentials credential count: %d for line: %s",
                 mCredentials.entries(), mIdentity.toString().data());
#endif

   SipLineCredentials* credential = (SipLineCredentials*) mCredentials.find(&matchRealm);
   if (credential)
   {
#ifdef TEST_PRINT
      OsSysLog::add(FAC_AUTH, PRI_DEBUG, "SipLine::getCredentials found credentials for realm: <%s>", matchRealm.data());
#endif
      credential->getUserId(userID);
      credential->getPasswordToken(&userPassword);
      credentialsFound = TRUE;
      credential = NULL;
      HttpMessage::buildMd5UserPasswordDigest(userID->data(), matchRealm.data(), userPassword.data(), *MD5_token);
   }
   else
   {
      credential = (SipLineCredentials*) mCredentials.find(&emptyRealm);
      if (credential)
      {
#ifdef TEST_PRINT
          OsSysLog::add(FAC_AUTH, PRI_DEBUG, "SipLine::getCredentials found credentials for realm: <%s>", emptyRealm.data());
#endif
          credential->getUserId(userID);
          credential->getPasswordToken(&userPassword);
          credentialsFound = TRUE;
          credential = NULL;
          HttpMessage::buildMd5UserPasswordDigest(userID->data(), realm.data(), userPassword.data(), *MD5_token);
      }
#ifdef TEST_PRINT
      else
      {
          OsSysLog::add(FAC_AUTH, PRI_DEBUG, "SipLine::getCredentials found no credentials");
      }
#endif
   }

   return  credentialsFound ;
}

UtlBoolean SipLine::getDuplicateCredentials(const UtlString& type /*[in]*/,
                                            const UtlString& realm /*[in]*/)
{
   UtlBoolean credentialsFound = FALSE;
   UtlString matchRealm(realm);
   UtlString emptyRealm(NULL);

   if (realm.length() == 0)
   {
      SipLineCredentials* credential = (SipLineCredentials*) mCredentials.find(&emptyRealm);
      if (credential)
      {
          credentialsFound = TRUE;
          credential = NULL;
      }
   }
   else
   {
      SipLineCredentials* credential = (SipLineCredentials*) mCredentials.find(&matchRealm);
      if (credential)
      {
         credentialsFound = TRUE;
         credential = NULL;
      }
   }

   return  credentialsFound ;
}

UtlBoolean SipLine::removeCredential(const UtlString *realm)
{
   UtlString matchRealm(*realm);

#if 0
   OsSysLog::add(FAC_SIP, PRI_INFO,
                 "SipLine::removeCredential mCredentials contains: %d credentials for realm: \"%s\"",
                 mCredentials.entries(), matchRealm.data()); 
#endif

   UtlContainable* wasRemoved = mCredentials.remove(&matchRealm);

   if(wasRemoved)
   {
      delete wasRemoved;
   }

   return(wasRemoved != NULL);
}

void SipLine::removeAllCredentials()
{
    mCredentials.destroyAll();

}


void SipLine::setPreferredContactUri(const Url& preferredContactUri)
{
    mPreferredContactUri = preferredContactUri ;
}


UtlBoolean SipLine::getPreferredContactUri(Url& preferredContactUri) const
{
    UtlString host ; 

    preferredContactUri = mPreferredContactUri ;
    preferredContactUri.getHostAddress(host) ;

    return (host.length() > 0) ;
}


LINE_CONTACT_TYPE SipLine::getContactType() const 
{
    return meContactType ;
}


void SipLine::setContactType(LINE_CONTACT_TYPE eContactType) 
{
    meContactType = eContactType ;
}


UtlBoolean SipLine::getAllCredentials( int MaxEnteries/*[in]*/ ,
      int& actualEnteries /*[out/int]*/,
      UtlString realm[]/*[out/int]*/,
      UtlString userId[]/*[out/int]*/,
      UtlString type[]/*[out/int]*/,
      UtlString passtoken[]/*[out/int]*/)
{
    UtlBoolean credentialsFound = FALSE;
    UtlString Realm;
    UtlString UserID;
    UtlString Type;
    UtlString Password;
    UtlString PassToken;
    int i = 0;

    UtlHashBagIterator observerIterator(mCredentials);
    SipLineCredentials* credential = NULL;

    do
    {
        credential = (SipLineCredentials*) observerIterator();
        if ( credential)
        {
            credential->getRealm(&Realm);
            credential->getUserId(&UserID);
            credential->getType(&Type);
            credential->getPasswordToken(&Password) ;
            PassToken = "";
            HttpMessage::buildMd5UserPasswordDigest(UserID.data(), Realm.data(), Password.data(), PassToken);
            
            realm[i].remove(0);
            realm[i].append(Realm);
            
            userId[i].remove(0);
            userId[i].append(UserID);

            type[i].remove(0);
            type[i].append(Type);

            passtoken[i].remove(0) ;
            passtoken[i].append(PassToken) ;

            i++;
            credentialsFound = TRUE;
        }
    }
    while(credential != NULL && i< MaxEnteries) ;
    actualEnteries = i;
    return  credentialsFound ;
}



UtlBoolean SipLine::IsDuplicateRealm(const UtlString realm, const UtlString scheme)
{
   UtlString userID;
   UtlString passToken;
   if (getDuplicateCredentials(scheme, realm))
      return TRUE;
   else
      return FALSE;
}

void SipLine::generateLineID(UtlString& lineId)
{
   NetMd5Codec::encode(mIdentity.toString().data(), lineId);

   // Shorten the line Ids to 12 chars (from 32)
   lineId.remove(12) ;
}
