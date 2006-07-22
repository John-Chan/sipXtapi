/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phone;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.io.IOUtils;
import org.apache.velocity.app.VelocityEngine;
import org.sipfoundry.sipxconfig.common.DataCollectionUtil;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.common.UserException;
import org.sipfoundry.sipxconfig.device.DeviceVersion;
import org.sipfoundry.sipxconfig.device.VelocityProfileGenerator;
import org.sipfoundry.sipxconfig.setting.BeanWithGroups;
import org.sipfoundry.sipxconfig.setting.Setting;

/**
 * Base class for managed phone subclasses
 */
public class Phone extends BeanWithGroups {
    // public because of checkstyle
    public static final String PHONE_CONSTANT = "phone";

    public static final PhoneModel MODEL = new PhoneModel("unmanagedPhone", "Unmanaged phone");

    public static final String GROUP_RESOURCE_ID = PHONE_CONSTANT;

    private String m_description;

    private String m_serialNumber;

    private PhoneModel m_model;

    private List<Line> m_lines = Collections.EMPTY_LIST;

    private PhoneContext m_phoneContext;

    private String m_tftpRoot;

    private VelocityEngine m_velocityEngine;

    private SipService m_sip;

    private String m_phoneTemplate;

    private String m_webDirectory;

    private String m_beanId;
    
    private DeviceVersion m_version;

    public Phone() {
        this(MODEL);
    }

    protected Phone(String beanId) {
        m_beanId = beanId;
    }

    protected Phone(PhoneModel model) {
        m_beanId = model.getBeanId();
        m_model = model;
    }

    @Override
    public void initialize() {
    }

    public String getModelLabel() {
        return m_model.getLabel();
    }

    public void setModelId(String modelId) {
        m_model = PhoneModel.getModel(getBeanId(), modelId);
    }

    public PhoneModel getModel() {
        return m_model;
    }

    public DeviceVersion getDeviceVersion() {
        return m_version;
    }

    public void setDeviceVersion(DeviceVersion version) {
        m_version = version;
    }

    public String getWebDirectory() {
        return m_webDirectory;
    }

    public void setWebDirectory(String webDirectory) {
        m_webDirectory = webDirectory;
    }

    public void setSipService(SipService sip) {
        m_sip = sip;
    }

    public SipService getSipService() {
        return m_sip;
    }

    public String getTftpRoot() {
        return m_tftpRoot;
    }

    public void setTftpRoot(String tftpRoot) {
        m_tftpRoot = tftpRoot;
    }

    public VelocityEngine getVelocityEngine() {
        return m_velocityEngine;
    }

    public void setVelocityEngine(VelocityEngine velocityEngine) {
        m_velocityEngine = velocityEngine;
    }

    protected Setting loadSettings() {
        Set defines = getModelDefinitions();
        return getModelFilesContext().loadDynamicModelFile("phone.xml", getModel().getBeanId(),
                defines);
    }

    protected Setting loadLineSettings() {
        Set defines = getModelDefinitions();
        return getModelFilesContext().loadDynamicModelFile("line.xml", getModel().getBeanId(),
                defines);
    }
    
    /**
     * When loading the settings model.
     * 
     * Example If you can add "not-extinct" the following setting will not
     * be loaded. Phone model id and version are added by default.
     *
     *  &lt;setting name="dinosaur" unless="not-extinct"/&gt;
     * 
     * @return
     */
    protected Set getModelDefinitions() {
        Set definitions = new HashSet();
        PhoneModel model = getModel();
        definitions.add(model.getModelId());
        if (getDeviceVersion() != null) {
            definitions.add(getDeviceVersion().getVersionId());
        }        
        return definitions;
    }

    /**
     * Each subclass must decide how as much of this generic line information translates into its
     * own setting model.
     */
    @SuppressWarnings("unused")
    protected void setLineInfo(Line line, LineInfo lineInfo) {
    }

    /**
     * Each subclass must decide how as much of this generic line information can be contructed
     * from its own setting model.
     */
    @SuppressWarnings("unused")
    protected LineInfo getLineInfo(Line line) {
        return null;
    }

    /**
     * Overwrite to generate profiles
     * 
     * @return null - which means do not generate profiles
     */
    public String getPhoneFilename() {
        return null;
    }

    public void generateProfiles() {
        String profileFileName = getPhoneFilename();
        if (profileFileName == null) {
            return;
        }

        Writer wtr = null;
        try {
            File file = new File(profileFileName);
            VelocityProfileGenerator.makeParentDirectory(file);
            wtr = new FileWriter(file);
            generateProfile(wtr);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            IOUtils.closeQuietly(wtr);
        }
    }

    public Line findByUsername(String username) {
        for (Line l : getLines()) {
            User user = l.getUser();
            if (user != null && user.getUserName().equals(username)) {
                return l;
            }
        }
        return null;
    }

    public Line findByUri(String uri) {
        for (Line l : getLines()) {
            String candidate = l.getUri();
            if (candidate.equals(uri)) {
                return l;
            }
        }
        return null;
    }

    public void removeLine(Line line) {
        getLines().remove(line);
        DataCollectionUtil.updatePositions(getLines());
    }

    /**
     * Removes profiles generated by generateProfiles
     */
    public void removeProfiles() {
        String profileFileName = getPhoneFilename();
        if (profileFileName == null) {
            return;
        }
        VelocityProfileGenerator.removeProfileFiles(new File[] {
            new File(profileFileName)
        });
    }

    public String getPhoneTemplate() {
        return m_phoneTemplate;
    }

    public void setPhoneTemplate(String phoneTemplate) {
        m_phoneTemplate = phoneTemplate;
    }

    public void generateProfile(Writer out) {
        if (getPhoneTemplate() == null) {
            return;
        }
        VelocityProfileGenerator profile = new VelocityProfileGenerator(this);
        generateProfile(profile, getPhoneTemplate(), out);
    }

    protected void generateProfile(VelocityProfileGenerator cfg, String template, Writer out) {
        cfg.setVelocityEngine(getVelocityEngine());
        cfg.generateProfile(template, out);
    }

    /**
     * @throws RestartException if it cannot complete the operation
     */
    public void restart() {
    }

    protected void sendCheckSyncToFirstLine() {
        if (getLines().size() == 0) {
            throw new RestartException("Restart command is sent to first line and "
                    + "first phone line is not valid");
        }

        Line line = getLine(0);

        // XCF-1059
        String outboundProxy = m_phoneContext.getPhoneDefaults().getDomainName();

        String outboundProxyPort = m_phoneContext.getPhoneDefaults().getProxyServerSipPort();
        m_sip.sendCheckSync(line.getUri(), outboundProxy, outboundProxyPort);
    }

    /**
     * @return ids used in PhoneFactory
     */
    public String getBeanId() {
        return m_beanId;
    }

    /**
     * Internal, do not call this method. Hibnerate property declared update=false, but still
     * required method be defined.
     */
    @SuppressWarnings("unused")
    public void setBeanId(String illegal_) {
    }

    public String getModelId() {
        return m_model.getModelId();
    }

    public String getDescription() {
        return m_description;
    }

    public void setDescription(String description) {
        m_description = description;
    }

    public String getSerialNumber() {
        return m_serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        m_serialNumber = cleanSerialNumber(serialNumber);
    }

    public static String cleanSerialNumber(String rawNumber) {
        if (rawNumber == null) {
            return null;
        }
        String clean = rawNumber.toLowerCase();
        clean = clean.replaceAll("[:\\s]*", "");

        return clean;
    }

    public List<Line> getLines() {
        return m_lines;
    }

    public void setLines(List<Line> lines) {
        m_lines = lines;
    }

    public void addLine(Line line) {
        if (m_lines == Collections.EMPTY_LIST) {
            m_lines = new ArrayList<Line>();
        }
        int max = getModel().getMaxLineCount();
        if (m_lines.size() >= max) {
            throw new MaxLinesException("Maximum number of allowed lines is " + max);
        }
        line.setPhone(this);
        line.setPosition(m_lines.size());
        m_lines.add(line);
        line.initialize();
    }

    public static class MaxLinesException extends UserException {
        MaxLinesException(String msg) {
            super(msg);
        }
    }

    public Line getLine(int position) {
        return m_lines.get(position);
    }

    @SuppressWarnings("unused")
    public void initializeLine(Line line) {
    }

    public PhoneContext getPhoneContext() {
        return m_phoneContext;
    }

    public void setPhoneContext(PhoneContext phoneContext) {
        m_phoneContext = phoneContext;
        initialize();
    }

    public Line createLine() {
        Line line = new Line();
        line.setPhone(this);
        line.setModelFilesContext(getModelFilesContext());
        return line;
    }    
}
