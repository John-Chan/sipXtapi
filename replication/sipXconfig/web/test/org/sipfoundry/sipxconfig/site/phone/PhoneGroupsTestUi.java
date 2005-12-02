/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.site.phone;

import junit.framework.Test;
import net.sourceforge.jwebunit.WebTestCase;

import org.sipfoundry.sipxconfig.site.SiteTestHelper;

public class PhoneGroupsTestUi extends WebTestCase {

    PhoneTestHelper tester;

    public static Test suite() throws Exception {
        return SiteTestHelper.webTestSuite(PhoneGroupsTestUi.class);
    }

    protected void setUp() throws Exception {
        getTestContext().setBaseUrl(SiteTestHelper.getBaseUrl());
        SiteTestHelper.home(getTester());
        tester = new PhoneTestHelper(getTester());
    }

    public void testDisplay() {
        tester.reset();
        tester.seedGroup(2);
        SiteTestHelper.home(getTester());
        clickLink("PhoneGroups");
        SiteTestHelper.assertNoException(getTester());
        String[][] table = new String[][] {
            {
                "1. seedGroup0", ""
            }, {
                "2. seedGroup1", ""
            },
        };
        assertTableRowsEqual("group:list", 2, table);
    }

    public void testMoveGroups() {
        tester.reset();
        tester.seedGroup(3);
        SiteTestHelper.home(getTester());
        clickLink("PhoneGroups");
        SiteTestHelper.enableCheckbox(getTester(), "selectedRow", 1, true);

        // move up
        clickButton("group:moveUp");
        SiteTestHelper.assertNoException(getTester());
        String[][] tableUp = new String[][] {
            {
                "1. seedGroup1", ""
            }, {
                "2. seedGroup0", ""
            }, {
                "3. seedGroup2", ""
            },
        };
        assertTableRowsEqual("group:list", 2, tableUp);

        // move down
        clickButton("group:moveDown");
        SiteTestHelper.assertNoException(getTester());
        String[][] tableDown = new String[][] {
            {
                "1. seedGroup0", ""
            }, {
                "2. seedGroup1", ""
            }, {
                "3. seedGroup2", ""
            },
        };
        assertTableRowsEqual("group:list", 2, tableDown);
    }

    public void testDelete() {
        tester.reset();
        tester.seedGroup(3);
        SiteTestHelper.home(getTester());
        clickLink("PhoneGroups");
        SiteTestHelper.enableCheckbox(getTester(), "selectedRow", 1, true);
        clickButton("group:delete");
        SiteTestHelper.assertNoException(getTester());
        String[][] table = new String[][] {
            {
                "1. seedGroup0", ""
            }, {
                "2. seedGroup2", ""
            },
        };
        assertTableRowsEqual("group:list", 2, table);
    }
}
