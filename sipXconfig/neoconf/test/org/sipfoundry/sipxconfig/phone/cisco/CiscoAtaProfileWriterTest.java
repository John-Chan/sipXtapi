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
package org.sipfoundry.sipxconfig.phone.cisco;

import java.io.StringWriter;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.setting.SettingImpl;
import org.sipfoundry.sipxconfig.setting.SettingSet;

public class CiscoAtaProfileWriterTest extends TestCase {
    private static final char LF = 0x0a;
    
    public void testWriteEntry() {
        StringWriter wtr = new StringWriter();
        CiscoAtaProfileWriter pwtr = new CiscoAtaProfileWriter(wtr);
        pwtr.writeEntry("bird", "crow");
        assertEquals("bird:crow" + LF, wtr.toString());
    }
    
    public void testWriteBitmap() {
        SettingSet birds = new SettingSet();
        SettingImpl bird = new SettingImpl(".BM.Bird");
        birds.addSetting(bird);
        SettingImpl bird0 = new SettingImpl(".BF.0.Bird");
        bird0.setValue("256");
        birds.addSetting(bird0);
        
        StringWriter wtr = new StringWriter();
        CiscoAtaProfileWriter pwtr = new CiscoAtaProfileWriter(wtr);
        birds.acceptVisitor(pwtr);
        
        assertEquals("Bird:0x100" + LF, wtr.toString());
    }

}