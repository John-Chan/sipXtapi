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
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;

import org.apache.commons.io.IOUtils;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.VelocityEngine;
import org.apache.velocity.exception.ParseErrorException;
import org.apache.velocity.exception.ResourceNotFoundException;

public class VxmlGenerator {
    
    private String m_promptsDirectory;
    
    private String m_scriptsDirectory;

    private VelocityEngine m_velocityEngine;
    
    private String m_template = "autoattendant.vm";

    public String getTemplate() {
        return m_template;
    }    

    public void setTemplate(String template) {
        m_template = template;
    }    

    public String getScriptsDirectory() {
        return m_scriptsDirectory;
    }

    public void setScriptsDirectory(String vxmlScriptsDirectory) {
        m_scriptsDirectory = vxmlScriptsDirectory;
    }

    public String getPromptsDirectory() {
        return m_promptsDirectory;
    }   

    public void setPromptsDirectory(String vxmlPromptsDirectory) {
        m_promptsDirectory = vxmlPromptsDirectory;
    }

    public VelocityEngine getVelocityEngine() {
        return m_velocityEngine;
    }
    
    public void setVelocityEngine(VelocityEngine velocityEngine) {
        m_velocityEngine = velocityEngine;
    }
    
    public void generate(AutoAttendant aa) {
        FileWriter out = null;
        try {
            File scriptsDir = new File(getScriptsDirectory());
            scriptsDir.mkdirs();
            File scriptsFile = new File(scriptsDir, aa.getScriptFileName());
            out = new FileWriter(scriptsFile);
            generate(aa, out);            
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            IOUtils.closeQuietly(out);
        }        
    }
    
    public void generate(AutoAttendant aa, Writer out) {
        try {
            VelocityContext context = new VelocityContext();
            context.put("attendant", aa);
            context.put("vxml", this);            
            getVelocityEngine().mergeTemplate(getTemplate(), context, out);
        } catch (ResourceNotFoundException e) {
            throw new RuntimeException(e);
        } catch (ParseErrorException e) {
            throw new RuntimeException(e);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
