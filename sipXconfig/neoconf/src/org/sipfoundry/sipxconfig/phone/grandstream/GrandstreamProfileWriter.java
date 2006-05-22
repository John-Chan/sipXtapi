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
package org.sipfoundry.sipxconfig.phone.grandstream;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collection;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingVisitor;

public class GrandstreamProfileWriter implements SettingVisitor {
    protected static final char LF = 0x0a;
    private OutputStream m_wtr;
    private GrandstreamPhone m_phone;
    private int m_lineIndex;
    
    GrandstreamProfileWriter(GrandstreamPhone phone) {
        m_phone = phone; 
    }
    
    public void write(OutputStream wtr) {
        setOutputStream(wtr);
        m_phone.getSettings().acceptVisitor(this);
        for (Line line : getLines()) {
            line.getSettings().acceptVisitor(this);
            m_lineIndex++;
        }
    }
    
    protected void setOutputStream(OutputStream wtr) {
        m_wtr = wtr;
    }
    
    protected OutputStream getOutputStream() {
        return m_wtr;
    }
    
    protected GrandstreamPhone getPhone() {
        return m_phone;
    }

    public void visitSetting(Setting setting) {
        writeProfileEntry(setting.getProfileName(), setting.getValue());
    }

    public void visitSettingGroup(Setting group) {
    }        

    String nonNull(String value) {
        return value == null ? StringUtils.EMPTY : value;
    }

    void writeProfileEntry(String name, String value) {
        if (isCompositeIpAddress(name)) {
            writeIpAddress(name, value);
        } else {
            writeLine(name, value);
        }
    }
    
    void writeLine(String name, String value) {
        String lname = name;
        if (isCompositeProfileName(lname)) {
            String[] names = name.split("-");
            lname = names[m_lineIndex];
        }
        String line = lname + " = " + nonNull(value) + (char) LF;
        try {
            getOutputStream().write(line.getBytes());
        } catch (IOException e) {
            throw new RuntimeException(e);
        }        
    }
    
    boolean isCompositeProfileName(String name) {
        return name.indexOf('-') >= 0;
    }
    
    boolean isCompositeIpAddress(String name) {
        return name.indexOf(',') >= 0;
    }
    
    void writeIpAddress(String name, String value) {
        String[] names = name.split(",");        
        String[] values = StringUtils.defaultString(value).split("\\.");
        for (int i = 0; i < names.length; i++) {
            String svalue = i < values.length ? values[i] : StringUtils.EMPTY;
            writeLine(names[i], svalue);
        }        
    }

    public Collection<Line> getLines() {
        int lineCount = getPhone().getModel().getMaxLineCount();
        Collection<Line> lines = new ArrayList(lineCount);
        lines.addAll(getPhone().getLines());

        // copy in blank lines of all unused lines
        for (int i = lines.size(); i < lineCount; i++) {
            Line line = getPhone().createLine();
            line.setPosition(i);
            lines.add(line);
        }

        return lines;
    }

}
