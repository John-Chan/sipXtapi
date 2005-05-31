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
package org.sipfoundry.sipxconfig.setting;

import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;
import java.util.Map;

import org.sipfoundry.sipxconfig.setting.type.BooleanSetting;
import org.sipfoundry.sipxconfig.setting.type.EnumSetting;
import org.sipfoundry.sipxconfig.setting.type.IntegerSetting;
import org.sipfoundry.sipxconfig.setting.type.SettingType;
import org.sipfoundry.sipxconfig.setting.type.StringSetting;

import junit.framework.TestCase;

public class SettingTypesTest extends TestCase {

    private Setting group;

    protected void setUp() throws Exception {
        XmlModelBuilder builder = new XmlModelBuilder("etc");
        InputStream in = getClass().getResourceAsStream("setting-types.xml");
        SettingGroup root = builder.buildModel(in);
        group = root.getSetting("group");
    }

    public void testSettingIntegerType() throws IOException {
        final int[][] EXPECTED = {
            {
                3, 15
            }, {
                0, Integer.MAX_VALUE
            }
        };

        final boolean[] EXPECTED_REQUIRED = {
            true, false
        };

        for (int i = 0; i < EXPECTED.length; i++) {
            int[] min_max = EXPECTED[i];

            Setting intSetting = group.getSetting("int_setting_" + i);
            SettingType type = intSetting.getType();
            assertTrue(type instanceof IntegerSetting);
            IntegerSetting intType = (IntegerSetting) type;
            assertEquals(min_max[0], intType.getMin());
            assertEquals(min_max[1], intType.getMax());
            assertEquals(EXPECTED_REQUIRED[i], intType.isRequired());
        }
    }

    public void testSettingStringType() throws IOException {
        Setting stringSetting = group.getSetting("str_setting_def");
        SettingType type = stringSetting.getType();
        assertTrue(type instanceof StringSetting);
        StringSetting strType = (StringSetting) type;
        assertEquals(256, strType.getMaxLen());
        assertNull(strType.getPattern());
        assertFalse(strType.isRequired());
        assertFalse(strType.isPassword());

        stringSetting = group.getSetting("str_setting");
        type = stringSetting.getType();
        assertTrue(type instanceof StringSetting);
        strType = (StringSetting) type;
        assertEquals(15, strType.getMaxLen());
        assertEquals("kuku", strType.getPattern());
        assertTrue(strType.isRequired());
        assertTrue(strType.isPassword());
    }

    public void testSettingEnumType() throws IOException {
        final String[][] V2L = {
                {
                    "0", "Zero"
                }, {
                    "something", "XXX"
                }, {
                    "no_label_value", "no_label_value"
                }
            };
        Setting enumSetting = group.getSetting("enum_setting");
        assertNotNull(enumSetting);
        SettingType type = enumSetting.getType();
        assertTrue(type instanceof EnumSetting);
        EnumSetting enumType = (EnumSetting) type;
        Map enums = enumType.getEnums();
        assertEquals(V2L.length, enums.size());
        int i = 0;
        for (Iterator iter = enums.entrySet().iterator(); iter.hasNext();i++) {
            Map.Entry entry = (Map.Entry) iter.next();
            assertEquals(V2L[i][0], entry.getKey());
            assertEquals(V2L[i][1], entry.getValue());
        }        
    }
    
    public void testDefaultBooleanType() {
        Setting setting = group.getSetting("boolean_default");
        SettingType type = setting.getType();

        assertTrue(type instanceof BooleanSetting);
        BooleanSetting boolSetting = (BooleanSetting) type;
        assertEquals("1", boolSetting.getTrueValue());
        assertEquals("0", boolSetting.getFalseValue());
    }

    public void testBooleanType() {
        Setting setting = group.getSetting("boolean_setting");
        SettingType type = setting.getType();

        assertTrue(type instanceof BooleanSetting);
        BooleanSetting boolSetting = (BooleanSetting) type;
        assertEquals("true", boolSetting.getTrueValue());
        assertEquals("false", boolSetting.getFalseValue());
    }
}
