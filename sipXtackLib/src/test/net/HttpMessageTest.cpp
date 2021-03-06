//
// Copyright (C) 2007-2010 SIPez LLC  All rights reserved.
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

#include <sipxunittests.h>
#include <sipxunit/TestUtilities.h>

#include <os/OsDefs.h>
#include <utl/UtlHashMapIterator.h>
#include <net/HttpMessage.h>
#include <net/SdpBody.h>
#include <net/HttpBody.h>


/**
 * Unittest for HttpMessage
 */
class HttpMessageTest : public SIPX_UNIT_BASE_CLASS
{
    CPPUNIT_TEST_SUITE(HttpMessageTest);
    CPPUNIT_TEST(testCreator);
    CPPUNIT_TEST(testMessage);
    CPPUNIT_TEST(testHeader);
    CPPUNIT_TEST(testContentIndirection);
    CPPUNIT_TEST(testSdp);
    CPPUNIT_TEST(testMd5Digest);
    CPPUNIT_TEST(testEscape);
    CPPUNIT_TEST(testNoHeaders);
    CPPUNIT_TEST_SUITE_END();

public:

    void testCreator()
    {
        const char *hdr = "\r\nContent-Length: 0\r\n\r\n";
        int expectedLen = strlen(hdr);
        HttpMessage* msg = new HttpMessage();
        UtlString buf;
        int bufLen = 0;

        msg->getBytes(&buf, &bufLen);
        ASSERT_STR_EQUAL_MESSAGE("Set header comes back intact", hdr, (char *)buf.data());
        CPPUNIT_ASSERT_MESSAGE("bytes should not be null", !buf.isNull());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buffer should contain only content length field",
                expectedLen, bufLen);
        ASSERT_STR_EQUAL_MESSAGE("buffer should contain only content length field",
                buf.data(), hdr);
        delete msg;
    }

    /**
     * Test header, message, body, message contructor
     */
    void testMessage()
    {
        // TODO break this up into several tests. To intertwined

        const char* name = "Content-Type";
        const char* value = "text/plain";
        const char* httpTopLine = "GET /index.html HTTP/1.0";
        const char* valueRef = NULL;
        const char* n2 = "yyy";
        const char* v2 = "yyy-value";
        const char* v2a = "yyy-value2";
        UtlString messageBytes;
        UtlString messageBytes2;
        int messageLen = 0;
        int messageLen2 = 0;
        const char* body = "<HTML>\n<H3>Hello\n<BR>\n</HTML>\n";
        const HttpBody *bodyRef;
        int bodyLength = 0;
        UtlString headerLinePart;
        HttpMessage *msg;
        HttpMessage *msg2;

        msg = new HttpMessage();

        // H E A D E R
        int fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be zero",
                0, fieldCount);

        msg->addHeaderField(name, value);
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be zero", 1,
                fieldCount);

        valueRef = msg->getHeaderValue(0);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value,
                valueRef);

        msg->setFirstHeaderLine(httpTopLine);
        valueRef = msg->getFirstHeaderLine();
        ASSERT_STR_EQUAL_MESSAGE("incorrect top header line value",
                valueRef, httpTopLine);

        valueRef = msg->getHeaderValue(0, name);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value,
            valueRef);

        msg->addHeaderField(n2, v2);
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 2", 2,
                fieldCount);

        valueRef = msg->getHeaderValue(0, n2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", v2,
            valueRef);

        msg->addHeaderField(n2, v2a);
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 3", 3,
                fieldCount);

        valueRef = msg->getHeaderValue(1, n2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", v2a,
            valueRef);

        // B O D Y
        HttpBody *httpBody = new HttpBody(body, strlen(body));
        msg->setBody(httpBody);
        bodyRef = msg->getBody();
        CPPUNIT_ASSERT_MESSAGE("bad body pointer", httpBody == bodyRef);

        bodyRef->getBytes(&valueRef, &bodyLength);
        CPPUNIT_ASSERT_MESSAGE("bad body pointer", valueRef != NULL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("incorrect body len", (int)strlen(body),
                bodyLength);
        ASSERT_STR_EQUAL_MESSAGE("incorrect body value", body, valueRef);

        const char* expectedLinePart[] = {
            "GET", "/index.html", "HTTP/1.0"
        };
        int n = sizeof(expectedLinePart) / sizeof(expectedLinePart[0]);

        for (int i = 0; i < n; i++)
        {
            msg->getFirstHeaderLinePart(i, &headerLinePart);
            CPPUNIT_ASSERT_MESSAGE("NULL header line part pointer",
                !headerLinePart.isNull());
            ASSERT_STR_EQUAL_MESSAGE("incorrect hdr line", expectedLinePart[i],
                headerLinePart.data());
            headerLinePart.remove(0);
        }

        msg->getBytes(&messageBytes, &messageLen);
        CPPUNIT_ASSERT_MESSAGE("NULL body pointer", !messageBytes.isNull());

        // message constructor
        msg2 = new HttpMessage(messageBytes.data(), messageLen);
        msg2->getBytes(&messageBytes2, &messageLen2);
        valueRef = msg2->getHeaderValue(0, name);
        ASSERT_STR_EQUAL_MESSAGE("incorrect message bytes", value, valueRef);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("incorrect message byte length",
                messageLen, messageLen2);
        delete msg2;

        delete msg;

        // AS DESIGNED: body delete is handled by delete msg
        // delete httpBody;
    }


    /**
     * Test header
     */
    void testHeader()
    {
        const char* name1 = "yyy";
        const char* value1 = "yyy-value";
        const char* valueRef = NULL;
        const char* name2 = "yyy1";
        const char* value2 = "yyy-value1";
        const char* value2a = "yyy-value2";
        const char* value2b = "yyy-value3";

	UtlBoolean  rc;

        HttpMessage *msg;

        msg = new HttpMessage();

        // H E A D E R
        int fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be zero",
                0, fieldCount);

        // add header field name1
        msg->addHeaderField(name1, value1);

        // get overall header field count 
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be zero", 1,
                fieldCount);

        // get header field count for name1
        fieldCount = msg->getCountHeaderFields(name1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be zero", 1,
                fieldCount);

        // get header field by index
        valueRef = msg->getHeaderValue(0);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value1,
                valueRef);

        // get header field by index and name
        valueRef = msg->getHeaderValue(0, name1);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value1,
                valueRef);

        // add header field name2
        msg->addHeaderField(name2, value2);

        // get header field by name and index
        valueRef = msg->getHeaderValue(0, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // add second header field name2
        msg->addHeaderField(name2, value2);

        // get header field by name and index
        valueRef = msg->getHeaderValue(0, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // set second header field name2
        msg->setHeaderValue(name2, value2b, 1);

        // get header field by name and index
        valueRef = msg->getHeaderValue(0, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // insert header field name2 as a second header with name2 
        msg->insertHeaderField(name2, value2a, 2);

        // get header field by name and index
        valueRef = msg->getHeaderValue(0, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // get overall header field count 
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 4", 4,
                fieldCount);

        // get name1 header field count 
        fieldCount = msg->getCountHeaderFields(name1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 1", 1,
                fieldCount);

        // get name2 header field count 
        fieldCount = msg->getCountHeaderFields(name2);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 3", 3,
                fieldCount);

        // get header field by name and index
        valueRef = msg->getHeaderValue(0, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // get header field by name and index
        valueRef = msg->getHeaderValue(1, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2a,
            valueRef);

        // get header field by index
        valueRef = msg->getHeaderValue(3);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2b,
            valueRef);

        // remove non-existing header field
        rc = msg->removeHeader("non-exist", 36);
        CPPUNIT_ASSERT_MESSAGE("incorrect return code", rc == FALSE);

        // get overall header field count 
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 4", 4,
                fieldCount);

        // remove header field name1
        rc = msg->removeHeader(name1, 0);
        CPPUNIT_ASSERT_MESSAGE("incorrect return code", rc == TRUE);

        // get header field by index
        valueRef = msg->getHeaderValue(0);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // get name1 header field count 
        fieldCount = msg->getCountHeaderFields(name1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 0", 0,
                fieldCount);

        // remove second header field name2
        rc = msg->removeHeader(name2, 1);
        CPPUNIT_ASSERT_MESSAGE("incorrect return code", rc == TRUE);

        // get overall header field count 
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 2", 2,
                fieldCount);

        // get name2 header field count 
        fieldCount = msg->getCountHeaderFields(name2);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 2", 2,
                fieldCount);

        // get header field by index
        valueRef = msg->getHeaderValue(0);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // get header field by name and index
        valueRef = msg->getHeaderValue(1, name2);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2b,
            valueRef);

        // get non-exist header field by name and index
        valueRef = msg->getHeaderValue(2, name2);
        CPPUNIT_ASSERT_MESSAGE("non-NULL field value", valueRef == NULL);

        // remove header field name2
        rc = msg->removeHeader(name2, 1);
        CPPUNIT_ASSERT_MESSAGE("incorrect return code", rc == TRUE);

        // get overall header field count 
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 1", 1,
                fieldCount);

        // get header field by index
        valueRef = msg->getHeaderValue(0);
        CPPUNIT_ASSERT_MESSAGE("NULL field value", valueRef != NULL);
        ASSERT_STR_EQUAL_MESSAGE("incorrect field value", value2,
            valueRef);

        // remove header field name2
        rc = msg->removeHeader(name2, 0);
        CPPUNIT_ASSERT_MESSAGE("incorrect return code", rc == TRUE);

        // get overall header field count 
        fieldCount = msg->getCountHeaderFields();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("field count should be 0", 0,
                fieldCount);

        delete msg;
    }

    void testContentIndirection()
    {
        const char* sip1 = "NOTIFY sip:1@a.com SIP/2.0\nContent-Type: message/external-body; access-type=\"URL\";expiration=\"Mon, 24 June 2002 09:00:00 GMT\";URL=\"http://www.example.com/the-indirect-content.au\";size=52723;hash=10AB568E91245681AC1B\n\nContent-Disposition: render\n";
        HttpMessage *msg1 = new HttpMessage(sip1);

        UtlString contentType;
        UtlString sdpType(CONTENT_TYPE_CONTENT_INDIRECTION);
        msg1->getContentType(&contentType);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, sdpType);

        UtlHashMap contentTypeParameters;
        contentType="";
        msg1->getContentType(&contentType, &contentTypeParameters);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid number of content-type parameters", 
            (int)contentTypeParameters.entries(), 5);
        
        //UtlHashMapIterator iterator(contentTypeParameters);
        UtlString* paramName = NULL;
        UtlString* paramValue = NULL;
        //while(paramName = (UtlString*)iterator())
        {
            //paramValue = (UtlString*)contentTypeParameters.findValue(paramName);
            //printf("%s=\"%s\"\n", paramName->data(), paramValue->data());
        }

        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, sdpType);
        UtlString paramKey;
        //printf("passed \"%s\"==\"%s\"\n", contentType.data(), sdpType.data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_ACCESS_TYPE;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type accessType parameter not found", paramValue);
        paramValue->toLower();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type accessType parameter", 
            (UtlString) HTTP_CONTENT_TYPE_PARAM_URL, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_URL;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type URL parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type URL parameter", 
            (UtlString) "http://www.example.com/the-indirect-content.au", *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_EXPIRATION;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type expiration parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type expiration parameter", 
            (UtlString) "Mon, 24 June 2002 09:00:00 GMT", *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_HASH;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type hash parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type hash parameter", 
            (UtlString) "10AB568E91245681AC1B", *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());
    
        paramKey = HTTP_CONTENT_TYPE_PARAM_SIZE;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type size parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type size parameter", 
            (UtlString) "52723", *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());


        const char* sip2="NOTIFY sip:1@a.com SIP/2.0\nContent-Type: message/external-body;access-type=\"URL\";expiration=\"Unlimited\";URL=\"https://iconfig135:7002/fttp-profile/profile?profileName=DEVICE&port=1&vendorDeviceID=B2-DBGM&aor=\";\nContent-ID: <B2-DBGM_1_DEVICE2007-09-05 00:00:00.0>";
        HttpMessage *msg2 = new HttpMessage(sip2);

        msg2->getContentType(&contentType);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, sdpType);

        contentTypeParameters.destroyAll();
        contentType="";
        //printf("reset contentTypeParameters has %d entries\n", contentTypeParameters.entries());
        msg2->getContentType(&contentType, &contentTypeParameters);
        //printf("msg2 has %d content-type parameters\n", contentTypeParameters.entries());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid number of content-type parameters", 
            (int)contentTypeParameters.entries(), 3);
        
        //UtlHashMapIterator iterator2(contentTypeParameters);
        paramName = NULL;
        paramValue = NULL;
        //while(paramName = (UtlString*)iterator2())
        {
            //paramValue = (UtlString*)contentTypeParameters.findValue(paramName);
            //printf("%s=\"%s\"\n", paramName->data(), paramValue->data());
        }

        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, sdpType);
        paramKey = "";;
        //printf("passed \"%s\"==\"%s\"\n", contentType.data(), sdpType.data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_ACCESS_TYPE;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type accessType parameter not found", paramValue);
        paramValue->toLower();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type accessType parameter", 
            (UtlString) HTTP_CONTENT_TYPE_PARAM_URL, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_URL;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type URL parameter not found", paramValue);
        //printf("testing %s=\"%s\"\n", paramKey.data(), paramValue->data());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type URL parameter", 
            (UtlString) "https://iconfig135:7002/fttp-profile/profile?profileName=DEVICE&port=1&vendorDeviceID=B2-DBGM&aor=", *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_EXPIRATION;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type expiration parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type expiration parameter", 
            (UtlString) "Unlimited", *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        HttpMessage msg3;
        UtlString expiration("Unlimited");
        UtlString contentUrl("http://m123054@config.example.com:1234/profiles?vendor=zenith&model=z-100&version=1.2.3&");
        int contentSize = 1234;
        UtlString contentSizeString;
        UtlInt::toString(contentSizeString, contentSize);
        UtlString hash("1234567890ABCDEF");
        msg3.setContentType(CONTENT_TYPE_CONTENT_INDIRECTION, 
                            HTTP_CONTENT_TYPE_PARAM_URL,
                            expiration,
                            contentUrl,
                            contentSize,
                            hash);

        const char* rawField = msg3.getHeaderValue(0, HTTP_CONTENT_TYPE_FIELD);
        CPPUNIT_ASSERT_MESSAGE("Could not find content-type header", rawField);
        //printf("Content-Type: =>%s<=\n", rawField);

        contentTypeParameters.destroyAll();
        contentType="";
        //printf("reset contentTypeParameters has %d entries\n", contentTypeParameters.entries());
        msg3.getContentType(&contentType, &contentTypeParameters);
        //printf("msg3 has %d content-type parameters\n", contentTypeParameters.entries());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid number of content-type parameters", 
            (int)contentTypeParameters.entries(), 5);
        
        //UtlHashMapIterator iterator3(contentTypeParameters);
        paramName = NULL;
        paramValue = NULL;
        //while(paramName = (UtlString*)iterator3())
        {
            //paramValue = (UtlString*)contentTypeParameters.findValue(paramName);
            //printf("%s=\"%s\"\n", paramName->data(), paramValue->data());
        }

        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, (UtlString) CONTENT_TYPE_CONTENT_INDIRECTION);
        paramKey = "";;
        //printf("passed \"%s\"==\"%s\"\n", contentType.data(), sdpType.data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_ACCESS_TYPE;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type accessType parameter not found", paramValue);
        paramValue->toLower();
        //printf("testing %s=\"%s\"\n", paramKey.data(), paramValue->data());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type accessType parameter", 
            (UtlString) HTTP_CONTENT_TYPE_PARAM_URL, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_URL;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type URL parameter not found", paramValue);
        //printf("testing %s=\"%s\"\n", paramKey.data(), paramValue->data());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type URL parameter", 
            contentUrl, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_EXPIRATION;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type expiration parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type expiration parameter", 
            expiration, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());

        paramKey = HTTP_CONTENT_TYPE_PARAM_HASH;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type hash parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type hash parameter", 
            hash, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());
    
        paramKey = HTTP_CONTENT_TYPE_PARAM_SIZE;
        paramValue = (UtlString*)contentTypeParameters.findValue(&paramKey);
        CPPUNIT_ASSERT_MESSAGE("Content-Type size parameter not found", paramValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Content-Type size parameter", 
            contentSizeString, *paramValue);
        //printf("passed %s=\"%s\"\n", paramKey.data(), paramValue->data());
    }

    void testSdp()
    {
        const char* sip = "INVITE 14 SIP/2.0\nContent-Type:application/sdp;fubar\n\n"
            "v=0\nm=audio 49170 RTP/AVP 0\nc=IN IP4 224.2.17.12/127";

        HttpMessage *msg = new HttpMessage(sip);
        SdpBody *sdp = (SdpBody *)msg->getBody();

        CPPUNIT_ASSERT_MESSAGE("Null sdp buffer", sdp != NULL);

        HttpBodyMultipart::BodyClassTypes bodyClassType = sdp->getClassType();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("HttpBody type incorrect", bodyClassType, HttpBodyMultipart::SDP_BODY_CLASS);

        int mediaCount = sdp->getMediaSetCount();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("incorrect media count", 1, mediaCount);

        UtlString contentType;
        UtlString sdpType("application/sdp");
        msg->getContentType(&contentType);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, sdpType);

        UtlHashMap contentTypeParameters;
        contentType="";
        msg->getContentType(&contentType, &contentTypeParameters);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("invalid content type string",
            contentType, sdpType);
        UtlString fubarName("fubar");
        CPPUNIT_ASSERT_MESSAGE("lost Content-Type header fubar parameter",
            contentTypeParameters.find(&fubarName));
        CPPUNIT_ASSERT_MESSAGE("invalid number of content-type parameters", 
            contentTypeParameters.entries() == 1);

        const char* referenceSdp = 
            "v=0\r\nm=audio 49170 RTP/AVP 0\r\nc=IN IP4 224.2.17.12/127\r\n";
        const char* sdpBytes = NULL;
        int sdpByteLength = 0;
        sdp->getBytes(&sdpBytes, &sdpByteLength);
        for(int iii = 0; iii < sdpByteLength; iii++)
        {
            if(referenceSdp[iii] != sdpBytes[iii])
            {
                printf("index[%d]: expected: %d got: %d\n",
                    iii, referenceSdp[iii], sdpBytes[iii]);
            }
        }
        CPPUNIT_ASSERT_MESSAGE("Null sdp serialized content", sdpBytes != NULL);
        CPPUNIT_ASSERT_MESSAGE("SDP does not match expected content",
            strcmp(referenceSdp, sdpBytes) == 0);

        HttpMessage* msgCopy = new HttpMessage(*msg);
        CPPUNIT_ASSERT_MESSAGE("NULL message copy", msgCopy != NULL);
        SdpBody *sdpCopy = (SdpBody *)msgCopy->getBody();
        CPPUNIT_ASSERT_MESSAGE("NULL SDP copy", sdpCopy != NULL);
        const char* sdpCopyBytes = NULL;
        int sdpCopyLen = 0;
        sdpCopy->getBytes(&sdpCopyBytes, &sdpCopyLen);
        //printf("SDP copy length: %d\n%s\n", sdpCopyLen, sdpCopyBytes);
        CPPUNIT_ASSERT_MESSAGE("Null sdp copy serialized content", sdpCopyBytes != NULL);
        CPPUNIT_ASSERT_MESSAGE("SDP does not match expected content",
            strcmp(referenceSdp, sdpCopyBytes) == 0);
    }


    void testMd5Digest()
    {
        const char* passToken = "john.salesman:sales@www/example.com:5+5=10";
        const char* nonce = "dcd98b7102dd2f0e8b11d0f600bfb0c093";
        const char* alg = HTTP_MD5_SESSION_ALGORITHM;
        const char *cnonce = NULL;
        int nonceCount = 0;
        const char* method = "GET";
        const char* qop = NULL;
        const char* uri = "/private/prices.html";
        const char *bodyDigest = NULL;
        const char* response = "739fc56a88db37aeebefe70572aeda5f";
        UtlString responseToken;

        HttpMessage::buildMd5Digest(passToken, alg, nonce, cnonce,  nonceCount,
                                   qop, method, uri, bodyDigest, &responseToken);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("httpmessage digest test",
            0, responseToken.compareTo(response));
    }

  void testEscape()
  {
    const char* umlautUriValue = "\"Thomas Frölich Snom 320\" <sip:2012@pforzheim.org;transport=udp>;tag=zmbmylm36n";
    
    UtlString umlautString(umlautUriValue);
    
    ASSERT_STR_EQUAL(umlautUriValue, umlautString.data());

    HttpMessage::escape(umlautString);
    const char* umlautUriEscaped = "%22Thomas+Fr%C3%B6lich+Snom+320%22+%3Csip%3A2012%40pforzheim.org%3Btransport%3Dudp%3E%3Btag%3Dzmbmylm36n";
    ASSERT_STR_EQUAL(umlautUriEscaped, umlautString.data());

    HttpMessage::unescape(umlautString);
    ASSERT_STR_EQUAL(umlautUriValue, umlautString.data());
  }

  void testNoHeaders()
  {
    UtlString sourceMessageBytes(END_OF_LINE_DELIMITOR END_OF_LINE_DELIMITOR);

    HttpMessage message(sourceMessageBytes);

    UtlString reconstructedMessageBytes;
    int messageLength;

    message.getBytes(&reconstructedMessageBytes, &messageLength);

    ASSERT_STR_EQUAL(sourceMessageBytes.data(), reconstructedMessageBytes.data());
    CPPUNIT_ASSERT_MESSAGE("message should be 4 bytes", messageLength == 4);
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(HttpMessageTest);
