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
package org.sipfoundry.sipxconfig.admin.callgroup;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.admin.dialplan.ForkQueueValue;

public class AbstractRingTest extends TestCase {

    public void testCalculateContact() {
        AbstractRing ring = new RingMock("444");

        ForkQueueValue q = new ForkQueueValue(3);
        ring.setExpiration(45);
        ring.setType(AbstractRing.Type.IMMEDIATE);

        String contact = ring.calculateContact("sipfoundry.org", q, false);
        assertEquals("<sip:444@sipfoundry.org?expires=45>;q=1.0", contact);

        AbstractRing ring2 = new RingMock("333");
        ring2.setExpiration(25);
        ring2.setType(AbstractRing.Type.DELAYED);
        String contact2 = ring2.calculateContact("sipfoundry.org", q, true);
        assertEquals("<sip:333@sipfoundry.org;sipx-noroute=Voicemail?expires=25>;q=0.95",
                contact2);

        // with new q value - ring2 is delayed, q mustbe < 1.0
        ForkQueueValue q1 = new ForkQueueValue(3);
        contact2 = ring2.calculateContact("sipfoundry.org", q1, false);
        assertEquals("<sip:333@sipfoundry.org?expires=25>;q=0.95", contact2);
    }

    private static final class RingMock extends AbstractRing {
        private final String m_userPart;

        public RingMock(String userPart) {
            m_userPart = userPart;
        }

        protected Object getUserPart() {
            return m_userPart;
        }
    }
}
