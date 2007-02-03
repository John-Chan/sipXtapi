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
package org.sipfoundry.sipxconfig.site.dialplan;

import junit.framework.Test;
import net.sourceforge.jwebunit.WebTestCase;

import org.sipfoundry.sipxconfig.site.SiteTestHelper;
import org.sipfoundry.sipxconfig.site.gateway.GatewaysTestUi;

/**
 * EditEmergencyRoutingTestUi
 */
public class EditEmergencyRoutingTestUi extends WebTestCase {
    public static Test suite() throws Exception {
        return SiteTestHelper.webTestSuite(EditEmergencyRoutingTestUi.class);
    }

    public void setUp() {
        getTestContext().setBaseUrl(SiteTestHelper.getBaseUrl());
        SiteTestHelper.home(getTester());
        clickLink("resetDialPlans");
        clickLink("EmergencyRouting");
    }

    public void testActivate() {
        SiteTestHelper.home(getTester());
        GatewaysTestUi.addTestGateways(getTester(), 3);
        SiteTestHelper.home(getTester());
        clickLink("EmergencyRouting");
        assertLinkPresent("erouting:addException");
        setFormElement("externalNumber", "33");
        clickButton("form:apply");
    }

    public void testAddException() throws Exception {
        SiteTestHelper.home(getTester());
        GatewaysTestUi.addTestGateways(getTester(), 3);
        SiteTestHelper.home(getTester());
        clickLink("EmergencyRouting");        
        setFormElement(SiteTestHelper.getIndexedId("externalNumber", 0), "12");
        clickLink("erouting:addException");
        SiteTestHelper.assertNoException(tester);
        SiteTestHelper.assertNoUserError(tester);
        tester.dumpResponse();
        setFormElement(SiteTestHelper.getIndexedId("externalNumber", 0), "33");
        setFormElement(SiteTestHelper.getIndexedId("externalNumber", 1), "911");
        setFormElement("callers", "11, 22");
        clickButton("form:apply");
    }
    
    public void testActivateWithErrors() {
        assertLinkPresent("erouting:addException");
        clickButton("form:apply");
        // activate dial plans page active
        SiteTestHelper.assertUserError(tester);
    }
}