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
package org.sipfoundry.sipxconfig.phone;

import java.util.Collection;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.TestHelper;


public class PhoneSummaryTestDb extends TestCase {

    private PhoneContext m_context;
    
    protected void setUp() throws Exception {
        m_context = (PhoneContext) TestHelper.getApplicationContext().getBean(
                PhoneContext.CONTEXT_BEAN_NAME);        
    }
    
    public void testLoad() throws Exception {        
        TestHelper.cleanInsert("ClearDb.xml");
        TestHelper.cleanInsertFlat("phone/dbdata/PhoneSummarySeed.xml");
        
        Collection summaries = m_context.loadPhones();
        
        assertEquals(3, summaries.size());
        Phone[] summariesArray = (Phone[]) summaries.toArray(new Phone[0]);

        assertEquals("unittest-sample phone1", summariesArray[0].getPhoneData().getDisplayLabel());
        assertEquals(1, summariesArray[0].getLines().size());

        assertEquals("unittest-sample phone2", summariesArray[1].getPhoneData().getDisplayLabel());
        assertEquals(0, summariesArray[1].getLines().size());

        assertEquals("unittest-sample phone3", summariesArray[2].getPhoneData().getDisplayLabel());
        assertEquals(2, summariesArray[2].getLines().size());
    }
}
