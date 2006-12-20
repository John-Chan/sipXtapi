/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.acd;

import java.util.Collections;

import junit.framework.TestCase;

import org.easymock.EasyMock;
import org.easymock.IMocksControl;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;

public class AcdAgentTest extends TestCase {

    private AcdAgent m_agent;
        
    protected void setUp() throws Exception {
        m_agent = (AcdAgent) TestHelper.getApplicationContext().getBean("acdAgent");
    }

    public void testGetSettings() throws Exception {
        assertNotNull(m_agent.getSettings());
    }

    public void testCalculateUri() {
        IMocksControl mc = EasyMock.createControl();
        CoreContext coreContext = mc.createMock(CoreContext.class);
        coreContext.getDomainName();
        mc.andReturn("mydomain.org");
        mc.replay();

        m_agent.setCoreContext(coreContext);

        User user = new User();
        user.setUserName("testuser");
        m_agent.setUser(user);

        assertEquals("sip:testuser@mydomain.org", m_agent.calculateUri());
    }

    public void testSettings() {
        IMocksControl mc = EasyMock.createControl();
        CoreContext coreContext = mc.createMock(CoreContext.class);
        coreContext.getDomainName();
        mc.andReturn("mydomain.org").atLeastOnce();
        mc.replay();

        m_agent.setCoreContext(coreContext);

        User user = new User();
        user.setUserName("testuser");
        m_agent.setUser(user);

        AcdQueue queue = new AcdQueue();
        queue.setCoreContext(coreContext);
        queue.setName("testqueue");
        queue.insertAgent(m_agent);

        m_agent.initialize();
        assertEquals("testuser", m_agent.getSettingValue(AcdAgent.NAME));
        assertEquals("sip:testuser@mydomain.org", m_agent.getSettingValue(AcdAgent.URI));
        assertEquals("sip:testqueue@mydomain.org", m_agent.getSettingValue(AcdAgent.QUEUE_LIST));

        user.setFirstName("aaa");
        user.setLastName("bbb");
        assertEquals("aaa bbb", m_agent.getSettingValue(AcdAgent.NAME));

        mc.verify();
    }

    public void testSettingsForMultipleQueues() {
        IMocksControl mc = EasyMock.createControl();
        CoreContext coreContext = mc.createMock(CoreContext.class);
        coreContext.getDomainName();
        mc.andReturn("mydomain.org").atLeastOnce();
        mc.replay();

        m_agent.setCoreContext(coreContext);

        User user = new User();
        user.setUserName("testuser");
        m_agent.setUser(user);

        AcdQueue queue1 = new AcdQueue();
        queue1.setUniqueId();
        queue1.setCoreContext(coreContext);
        queue1.setName("testqueue1");
        queue1.insertAgent(m_agent);

        AcdQueue queue2 = new AcdQueue();
        queue2.setUniqueId();
        queue2.setCoreContext(coreContext);
        queue2.setName("testqueue2");
        queue2.insertAgent(m_agent);

        m_agent.initialize();
        assertEquals("sip:testqueue1@mydomain.org,sip:testqueue2@mydomain.org", m_agent
                .getSettingValue(AcdAgent.QUEUE_LIST));

        mc.verify();
    }

    public void testMoveQueues() {
        AcdQueue queue1 = new AcdQueue();
        queue1.setUniqueId();
        queue1.setName("testqueue");
        queue1.insertAgent(m_agent);

        AcdQueue queue2 = new AcdQueue();
        queue2.setUniqueId();
        queue2.setName("testqueue");
        queue2.insertAgent(m_agent);

        assertEquals(queue1, m_agent.getQueues().get(0));
        m_agent.moveQueues(Collections.singletonList(queue2.getId()), -1);
        assertEquals(queue2, m_agent.getQueues().get(0));
        m_agent.moveQueues(Collections.singletonList(queue2.getId()), 1);
        assertEquals(queue1, m_agent.getQueues().get(0));
    }
}
