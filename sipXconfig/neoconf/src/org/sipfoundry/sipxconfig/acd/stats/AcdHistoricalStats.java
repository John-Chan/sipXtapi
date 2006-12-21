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
package org.sipfoundry.sipxconfig.acd.stats;

import java.util.List;
import java.util.Map;

public interface AcdHistoricalStats {
    
    public static final String BEAN_NAME = "acdHistoricalStats";

    public List<String> getReports();
    
    public List<String> getReportFields(String reportName);

    public List<Map<String, Object>> getReport(String name);
    
}
