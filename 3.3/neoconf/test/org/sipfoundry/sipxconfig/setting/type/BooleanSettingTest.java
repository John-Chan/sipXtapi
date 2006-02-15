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

public class BooleanSettingTest extends TestCase {

    public void testConvertToTypedValue() {
        SettingType type = new BooleanSetting();
        assertTrue(((Boolean) type.convertToTypedValue("1")).booleanValue());
        assertFalse(((Boolean) type.convertToTypedValue("0")).booleanValue());
        assertNull(type.convertToTypedValue(null));
        assertFalse(((Boolean) type.convertToTypedValue("xyz")).booleanValue());
    }

    public void testConvertToStringValue() {
        BooleanSetting type = new BooleanSetting();
        type.setTrueValue("enabled");
        type.setFalseValue("disabled");
        assertEquals("enabled", type.convertToStringValue(Boolean.TRUE));
        assertEquals("disabled", type.convertToStringValue(Boolean.FALSE));
        assertNull(type.convertToStringValue(null));
    }
}
