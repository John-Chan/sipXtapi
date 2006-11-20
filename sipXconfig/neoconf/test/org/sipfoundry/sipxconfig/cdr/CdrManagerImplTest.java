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
package org.sipfoundry.sipxconfig.cdr;

import java.sql.ResultSet;
import java.sql.Timestamp;
import java.util.List;

import org.easymock.EasyMock;
import org.easymock.IMocksControl;
import org.sipfoundry.sipxconfig.SipxDatabaseTestCase;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.cdr.Cdr.Termination;
import org.sipfoundry.sipxconfig.cdr.CdrManagerImpl.CdrsResultReader;
import org.springframework.context.ApplicationContext;

public class CdrManagerImplTest extends SipxDatabaseTestCase {
    
    // FIXME: reads from real CDR database - needs SIPXCDR_TEST
    public void _testGetCdrs() {
        ApplicationContext app = TestHelper.getApplicationContext();
        CdrManager cdrManager = (CdrManager) app.getBean(CdrManager.CONTEXT_BEAN_NAME);
        List<Cdr> cdrs = cdrManager.getCdrs(null, null);
        assertTrue(cdrs.size() > 0);
    }
    
    public void testProcessRow() throws Exception {
        IMocksControl rsControl = EasyMock.createControl();
        ResultSet rs = rsControl.createMock(ResultSet.class);
        
        rs.getString("caller_aor");
        rsControl.andReturn("caller");
        rs.getString("callee_aor");
        rsControl.andReturn("callee");
        
        rs.getTimestamp("start_time");
        rsControl.andReturn(new Timestamp(0));
        rs.getTimestamp("connect_time");
        rsControl.andReturn(new Timestamp(1));
        rs.getTimestamp("end_time");
        rsControl.andReturn(new Timestamp(2));
        
        rs.getInt("failure_status");
        rsControl.andReturn(404);
        
        rs.getString("termination");
        rsControl.andReturn("I");
        
        rsControl.replay();

        
        CdrsResultReader reader = new CdrManagerImpl.CdrsResultReader();
        reader.processRow(rs);
        
        List<Cdr> results = reader.getResults();
        assertEquals(1, results.size());
        Cdr cdr = results.get(0);
        assertEquals("callee", cdr.getCalleeAor());
        assertEquals("caller", cdr.getCallerAor());
        assertEquals(0, cdr.getStartTime().getTime());
        assertEquals(1, cdr.getConnectTime().getTime());
        assertEquals(2, cdr.getEndTime().getTime());        
        assertEquals(404, cdr.getFailureStatus());
        assertEquals(Termination.IN_PROGRESS, cdr.getTermination());
                
        rsControl.verify();
    }

}
