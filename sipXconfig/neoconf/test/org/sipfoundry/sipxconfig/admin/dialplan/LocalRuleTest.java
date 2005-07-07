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
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.util.Collections;
import java.util.List;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.admin.dialplan.config.FullTransform;
import org.sipfoundry.sipxconfig.admin.dialplan.config.Permission;
import org.sipfoundry.sipxconfig.admin.dialplan.config.Transform;
import org.sipfoundry.sipxconfig.gateway.Gateway;

/**
 * LocalRuleTest
 */
public class LocalRuleTest extends TestCase {
    private LocalRule m_rule;

    protected void setUp() throws Exception {
        m_rule = new LocalRule();
        m_rule.setPstnPrefix("9");
        m_rule.setExternalLen(7);
        
        Gateway g = new Gateway();
        g.setAddress("local.gateway.com");
        m_rule.setGateways(Collections.singletonList(g));        
    }

    public void testGetPatterns() {
        String[] patterns = m_rule.getPatterns();
        assertEquals(2, patterns.length);
        assertEquals("9xxxxxxx", patterns[0]);
        assertEquals("xxxxxxx", patterns[1]);
    }

    public void testGetTransforms() {
        Transform[] transforms = m_rule.getTransforms();
        assertEquals(1,transforms.length);
        FullTransform transform = (FullTransform) transforms[0];
        assertEquals("{vdigits}", transform.getUser());
        assertEquals("local.gateway.com", transform.getHost());
    }

    public void testGetPermissions() {
        List permissions = m_rule.getPermissions();
        assertEquals(1, permissions.size());
        assertEquals(Permission.LOCAL_DIALING, permissions.get(0));
    }
    
    public void testGetTranformedPatters() throws Exception {
        String[] tps = m_rule.getTransformedPatterns();
        assertEquals(1,tps.length);
        assertEquals("xxxxxxx",tps[0]);        
    }
}
