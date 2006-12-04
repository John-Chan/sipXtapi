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

import java.io.File;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.phone.LineInfo;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.PhoneContext;
import org.sipfoundry.sipxconfig.speeddial.SpeedDial;

public class ClearonePhone extends Phone {
    public static final String BEAN_ID = "clearone";
    public static final String CONFIG_FILE = "C1MAXIP_%s.txt";
    public static final String DIALPLAN_FILE = "c1dialplan_%s.txt";

    public ClearonePhone() {
        super(BEAN_ID);
    }

    public ClearonePhone(ClearoneModel model) {
        super(model);
    }

    @Override
    public String getPhoneTemplate() {
        return "clearone/C1MAXIP.txt.vm";
    }

    public String getDialplanTemplate() {
        return "clearone/c1dialplan.txt.vm";
    }

    @Override
    public void initializeLine(Line line) {
        ClearoneLineDefaults defaults = new ClearoneLineDefaults(line, getPhoneContext()
                .getPhoneDefaults());
        line.addDefaultBeanSettingHandler(defaults);
    }

    @Override
    public void initialize() {
        PhoneContext phoneContext = getPhoneContext();
        ClearonePhoneDefaults defaults = new ClearonePhoneDefaults(phoneContext
                .getPhoneDefaults(), formatName(DIALPLAN_FILE));
        addDefaultBeanSettingHandler(defaults);
    }

    @Override
    public void generateProfiles() {
        PhoneContext phoneContext = getPhoneContext();
        SpeedDial speedDial = phoneContext.getSpeedDial(this);
        if (speedDial != null) {
            ClearonePhoneSpeedDial phoneSpeedDial = new ClearonePhoneSpeedDial(speedDial);
            addDefaultSettingHandler(phoneSpeedDial);
        }
        super.generateProfiles();
        // generate some other files
        generateFile(getDialplanTemplate(), getDialplanFileName());
    }

    @Override
    public String getPhoneFilename() {
        return new File(getTftpRoot(), formatName(CONFIG_FILE)).getPath();
    }

    public String getDialplanFileName() {
        return new File(getTftpRoot(), formatName(DIALPLAN_FILE)).getPath();
    }

    private String formatName(String format) {
        // HACK: in some cases this function is called before serial number is assigned, it needs
        // to work with 'null' serial number
        String serialNumber = StringUtils.defaultString(getSerialNumber()).toUpperCase();
        return String.format(format, serialNumber);
    }

    @Override
    protected LineInfo getLineInfo(Line line) {
        return ClearoneLineDefaults.getLineInfo(this, line);
    }

    @Override
    protected void setLineInfo(Line line, LineInfo lineInfo) {
        ClearoneLineDefaults.setLineInfo(this, line, lineInfo);
    }
}
