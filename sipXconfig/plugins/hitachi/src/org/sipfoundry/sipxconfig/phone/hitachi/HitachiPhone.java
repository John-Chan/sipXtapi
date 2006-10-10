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
package org.sipfoundry.sipxconfig.phone.hitachi;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.Writer;

import org.apache.commons.io.IOUtils;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.phone.LineInfo;
import org.sipfoundry.sipxconfig.phone.Phone;

public class HitachiPhone extends Phone {
    public static final String BEAN_ID = "hitachi";
    private static final String USER_ID_SETTING = "USER_ACCOUNT/User_ID";
    private static final String DISPLAY_NAME_SETTING = "USER_ACCOUNT/Displayname";
    private static final String PASSWORD_SETTING = "USER_ACCOUNT/User_Password";
    private static final String REGISTRATION_SERVER_SETTING = "SERVER_SETTINGS/1st_Registrar";

    public HitachiPhone() {
        super(BEAN_ID);
    }

    public HitachiPhone(HitachiModel model) {
        super(model);
    }

    public String getLoadrunTemplate() {
        return "hitachi/loadrun.ini";
    }

    @Override
    public String getPhoneTemplate() {
        return "hitachi/user.ini.vm";
    }

    @Override
    public void initializeLine(Line line) {
        HitachiLineDefaults defaults = new HitachiLineDefaults(line);
        line.addDefaultBeanSettingHandler(defaults);
    }

    @Override
    public void initialize() {
        HitachiPhoneDefaults defaults = new HitachiPhoneDefaults(getPhoneContext()
                .getPhoneDefaults());
        addDefaultBeanSettingHandler(defaults);
    }

    @Override
    public void generateProfiles() {
        super.generateProfiles();
        // and copy loadrun.ini as well
        generateLoadrunIni(getTftpRoot());
    }

    void generateLoadrunIni(String dstDir) {
        Writer loadrunIniWriter = null;
        try {
            InputStream loadrunIniTemplate = getClass().getClassLoader().getResourceAsStream(
                    getLoadrunTemplate());
            File loadrunIniFile = new File(dstDir, "loadrun.ini");
            loadrunIniWriter = new FileWriter(loadrunIniFile);
            IOUtils.copy(loadrunIniTemplate, loadrunIniWriter);
        } catch (IOException e) {
            throw new RuntimeException("Cannot generate profiles for " + getSerialNumber(), e);
        } finally {
            IOUtils.closeQuietly(loadrunIniWriter);
        }
    }

    /**
     * Check loadrun.ini section [3] for a proper format of the file name.
     * 
     * %muser.ini means “3 bytes of MAC address + user.ini”
     */
    @Override
    public String getPhoneFilename() {
        String serialNumber = getSerialNumber();
        String prefix = serialNumber.substring(6);
        return getTftpRoot() + "/" + prefix + "user.ini";
    }

    @Override
    protected LineInfo getLineInfo(Line line) {
        LineInfo info = new LineInfo();
        info.setDisplayName(line.getSettingValue(DISPLAY_NAME_SETTING));
        info.setUserId(line.getSettingValue(USER_ID_SETTING));
        info.setPassword(line.getSettingValue(PASSWORD_SETTING));
        // phone setting
        info.setRegistrationServer(getSettingValue(REGISTRATION_SERVER_SETTING));
        return info;
    }

    @Override
    protected void setLineInfo(Line line, LineInfo lineInfo) {
        line.setSettingValue(DISPLAY_NAME_SETTING, lineInfo.getDisplayName());
        line.setSettingValue(USER_ID_SETTING, lineInfo.getUserId());
        line.setSettingValue(PASSWORD_SETTING, lineInfo.getPassword());
        // phone setting
        setSettingValue(REGISTRATION_SERVER_SETTING, lineInfo.getRegistrationServer());
    }
}
