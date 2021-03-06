//
// Copyright (C) 2007-2010 SIPez LLC  All rights reserved.
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

#include <os/OsRWMutex.h>
#include <os/OsMutex.h>
#include <sipxunittests.h>

class OsMutexTest : public SIPX_UNIT_BASE_CLASS
{
    CPPUNIT_TEST_SUITE(OsMutexTest);
    CPPUNIT_TEST(testMutex);
    CPPUNIT_TEST(testRWMutex);
    CPPUNIT_TEST_SUITE_END();


public:
    void testMutex()
    {
        OsMutex* pMutex;

        pMutex = new OsMutex(OsMutex::Q_FIFO);
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pMutex->acquire());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pMutex->acquire(100));
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pMutex->tryAcquire());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pMutex->release());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pMutex->release());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pMutex->release());

        // TBD: test mutex locked by another thread!
        delete pMutex;    
    }

    void testRWMutex()
    {
        OsRWMutex* pRWMutex;

        pRWMutex = new OsRWMutex(OsRWMutex::Q_FIFO);
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pRWMutex->acquireRead());
        CPPUNIT_ASSERT_EQUAL(OS_BUSY, pRWMutex->tryAcquireWrite());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pRWMutex->releaseRead());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pRWMutex->tryAcquireWrite());
        CPPUNIT_ASSERT_EQUAL(OS_BUSY, pRWMutex->tryAcquireRead());
        CPPUNIT_ASSERT_EQUAL(OS_SUCCESS, pRWMutex->releaseWrite());
        delete pRWMutex;    
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(OsMutexTest);

