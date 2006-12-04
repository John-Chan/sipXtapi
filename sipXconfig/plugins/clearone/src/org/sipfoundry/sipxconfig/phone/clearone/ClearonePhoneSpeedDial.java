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
package org.sipfoundry.sipxconfig.phone.clearone;

import java.util.List;

import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingValue2;
import org.sipfoundry.sipxconfig.setting.SettingValueHandler;
import org.sipfoundry.sipxconfig.setting.SettingValueImpl;
import org.sipfoundry.sipxconfig.speeddial.Button;
import org.sipfoundry.sipxconfig.speeddial.SpeedDial;

public class ClearonePhoneSpeedDial implements SettingValueHandler {
    private static final String PREFIX = "speed_dial/speed_dial_";

    private SpeedDial m_speedDial;

    public ClearonePhoneSpeedDial(SpeedDial speedDial) {
        m_speedDial = speedDial;
    }

    public SettingValue2 getSettingValue(Setting setting) {
        int index = indexFromSettingPath(setting.getProfilePath());
        List<Button> buttons = m_speedDial.getButtons();
        if (index < 0 || index >= buttons.size()) {
            return null;
        }
        String number = buttons.get(index).getNumber();
        return new SettingValueImpl(number);
    }

    private int indexFromSettingPath(String path) {
        if (path.startsWith(PREFIX)) {
            String index = path.substring(PREFIX.length());
            return Integer.parseInt(index);
        }
        return -1;
    }
}
