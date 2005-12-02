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
package org.sipfoundry.sipxconfig.common;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;

import junit.framework.TestCase;

public class CollectionUtilsTest extends TestCase {
    
    private Collection nonEmptyCollection;

    protected void setUp() throws Exception {
        super.setUp();

        nonEmptyCollection = new ArrayList();
        nonEmptyCollection.add(new Object());
    }
    
    public void testSafeSize() {
        assertEquals(0, CollectionUtils.safeSize(null));
        assertEquals(1, CollectionUtils.safeSize(nonEmptyCollection));
    }
    
    public void testSafeIterator() {
        Iterator iter = CollectionUtils.safeIterator(null);
        assertFalse(iter.hasNext());
        
        iter = CollectionUtils.safeIterator(nonEmptyCollection);
        assertTrue(iter.hasNext());
    }

}
