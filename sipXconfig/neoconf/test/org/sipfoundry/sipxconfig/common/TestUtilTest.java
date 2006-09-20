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
package org.sipfoundry.sipxconfig.common;

import java.io.File;
import java.util.Properties;

import org.sipfoundry.sipxconfig.test.TestUtil;

import junit.framework.TestCase;


public class TestUtilTest extends TestCase {
    
    public void testGetClasspathDirectory() {
        String classpathDir = TestUtil.getClasspathDirectory(TestUtilTest.class);
        File thisClassFile = new File(classpathDir + "/org/sipfoundry/sipxconfig/common/TestUtilTest.class");
        assertTrue(thisClassFile.exists());
    }
    
    public void testGetProjectDirectory() {
        String projectDirectory = TestUtil.getProjectDirectory();
        assertEquals("neoconf", new File(projectDirectory).getName());
    }
    
    public void testGetSysDirProperties() {
        Properties props = new Properties();
        TestUtil.setSysDirProperties(props, "etc", "out");
        assertEquals("etc", props.getProperty("sysdir.etc"));
        assertEquals("out", props.getProperty("sysdir.data"));
        assertEquals("out", props.getProperty("sysdir.phone"));
        assertEquals("out", props.getProperty("sysdir.log"));
    }
    
    public void testGetTopBuildDirectory() {
        String buildDir = TestUtil.getBuildDirectory("neoconf");
        File topBuild = new File(buildDir).getParentFile();
        File configProperties = new File(topBuild, "build.properties");
        assertTrue(configProperties.exists());
    }
}
