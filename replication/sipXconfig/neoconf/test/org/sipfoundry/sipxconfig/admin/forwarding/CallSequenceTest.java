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
package org.sipfoundry.sipxconfig.admin.forwarding;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.common.User;

/**
 * CallSequenceTest
 */
public class CallSequenceTest extends TestCase {
    private User m_user;

    protected void setUp() throws Exception {
        m_user = new User();
        m_user.setUserName("abc");
    }

    public void testGenerateAliases() {
        final int N = 7;
        List rings = new ArrayList(N);
        for (int i = 0; i < N; i++) {
            Ring ring = new Ring("2" + i, i, Ring.Type.DELAYED);
            rings.add(ring);
        }
        // add empty ring - should not change anything
        rings.add(new Ring());

        CallSequence sequence = new CallSequence();
        sequence.setUser(m_user);
        sequence.setCalls(rings);

        List aliases = sequence.generateAliases("sipfoundry.org");
        assertEquals(N, aliases.size());
        for (Iterator i = aliases.iterator(); i.hasNext();) {
            AliasMapping a = (AliasMapping) i.next();
            assertEquals("abc@sipfoundry.org", a.getIdentity());
            String contact = a.getContact();
            assertTrue(contact
                    .matches("<sip:\\d+@sipfoundry.org;sipx-noroute=Voicemail\\?expires=\\d+>;q=[01]\\.\\d+"));
        }
    }

    public void testGenerateAuthExceptions() {
        final int N = 7;
        List rings = new ArrayList(N);
        for (int i = 0; i < N; i++) {
            Ring ring = new Ring("2" + i, i, Ring.Type.DELAYED);
            rings.add(ring);
        }
        // add empty ring - should not change anything
        rings.add(new Ring());

        CallSequence sequence = new CallSequence();
        sequence.setUser(m_user);
        sequence.setCalls(rings);

        List authExceptions = sequence.generateAuthExceptions();
        assertEquals(N, authExceptions.size());
        for (int i = 0; i < authExceptions.size(); i++) {
            String number = (String) authExceptions.get(i);
            assertEquals("2" + i, number);
        }
    }

    public void testGenerateAliasesEmpty() {
        CallSequence sequence = new CallSequence();
        sequence.setUser(m_user);
        List list = sequence.generateAliases("sipfoundry.org");
        assertEquals(0, list.size());
    }

    public void testMove() {
        CallSequence callSequence = new CallSequence();

        List calls = new ArrayList();

        Ring ring0 = new Ring("000", 40, Ring.Type.IMMEDIATE);
        Ring ring1 = new Ring("111", 40, Ring.Type.IMMEDIATE);
        Ring ring2 = new Ring("222", 40, Ring.Type.IMMEDIATE);

        calls.add(ring0.setUniqueId());
        calls.add(ring1.setUniqueId());
        calls.add(ring2.setUniqueId());

        callSequence.setCalls(calls);
        assertTrue(callSequence.moveRingDown(ring0));
        assertTrue(callSequence.moveRingDown(ring0));
        assertTrue(callSequence.moveRingUp(ring2));
        calls = callSequence.getCalls();

        assertSame(ring2, calls.get(0));
        assertSame(ring1, calls.get(1));
        assertSame(ring0, calls.get(2));
    }
}
