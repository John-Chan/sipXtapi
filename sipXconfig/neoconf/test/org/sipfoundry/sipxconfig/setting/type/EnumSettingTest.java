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
package org.sipfoundry.sipxconfig.setting.type;

import junit.framework.TestCase;

public class EnumSettingTest extends TestCase {

    private EnumSetting m_intEnum;
    private EnumSetting m_stringEnum;

    protected void setUp() throws Exception {
        m_intEnum = new EnumSetting();
        m_intEnum.addEnum("1", "jeden");
        m_intEnum.addEnum("2", "dwa");
        m_intEnum.addEnum("3", "trzy");

        m_stringEnum = new EnumSetting();
        m_stringEnum.addEnum("one", "jeden");
        m_stringEnum.addEnum("two", "dwa");
        m_stringEnum.addEnum("three", "trzy");
    }

    public void testConvertToTypedValue() {
        assertEquals(new Integer(1), m_intEnum.convertToTypedValue("1"));
        assertNull(m_intEnum.convertToTypedValue("bongo"));
        assertNull(m_intEnum.convertToTypedValue(null));

        assertEquals("one", m_stringEnum.convertToTypedValue("one"));
        assertNull(m_stringEnum.convertToTypedValue("bongo"));
        assertNull(m_stringEnum.convertToTypedValue(null));
    }

    public void testConvertToStringValue() {
        assertEquals("2", m_intEnum.convertToStringValue(new Integer(2)));
        assertNull(m_intEnum.convertToStringValue(new Integer(5)));
        assertNull(m_intEnum.convertToStringValue(null));

        assertEquals("two", m_stringEnum.convertToStringValue("two"));
        assertNull(m_stringEnum.convertToStringValue("five"));
        assertNull(m_stringEnum.convertToStringValue(null));        
    }
}
