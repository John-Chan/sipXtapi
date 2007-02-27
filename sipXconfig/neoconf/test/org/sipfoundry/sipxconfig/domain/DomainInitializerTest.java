/*
 * 
 * 
 * Copyright (C) 2006 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2006 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.domain;

import junit.framework.TestCase;

import org.easymock.EasyMock;
import org.easymock.IMocksControl;
import org.sipfoundry.sipxconfig.common.InitializationTask;

public class DomainInitializerTest extends TestCase {
    private DomainInitializer m_domainInitializer;
    
    protected void setUp() {
        m_domainInitializer = new DomainInitializer();
    }

    public void testGetLocalFQDN() {
        assertNotNull(m_domainInitializer.getInitialDomainName());
        m_domainInitializer.setInitialDomain("bluejay");
        assertEquals("bluejay", m_domainInitializer.getInitialDomainName());
    }
    
    public void testOnApplicationEvent() {
        Domain domain = new Domain();   
        domain.setName("sparrow");
        IMocksControl domainManagerControl = EasyMock.createControl();
        DomainManager domainManager = domainManagerControl.createMock(DomainManager.class);
        domainManager.saveDomain(domain);
        domainManagerControl.replay();
        
        m_domainInitializer.setInitialDomain("sparrow");
        m_domainInitializer.setDomainManager(domainManager);
        InitializationTask init = new InitializationTask("initialize-domain");
        m_domainInitializer.onApplicationEvent(init);

        domainManagerControl.verify();
    }
}