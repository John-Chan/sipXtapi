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
package org.sipfoundry.sipxconfig.gateway;

import java.io.Serializable;
import java.util.Collection;
import java.util.Iterator;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.sipfoundry.sipxconfig.job.JobContext;
import org.sipfoundry.sipxconfig.phone.ProfileManager;

public class GatewayProfileManagerImpl implements ProfileManager {
    private static final Log LOG = LogFactory.getLog(GatewayProfileManagerImpl.class);

    private GatewayContext m_gatewayContext;

    private JobContext m_jobContext;

    public void generateProfileAndRestart(Integer gatewayId) {
        Gateway g = m_gatewayContext.getGateway(gatewayId);
        Serializable jobId = m_jobContext.schedule("Projection for gateway " + g.getName());
        try {
            m_jobContext.start(jobId);
            g.generateProfiles();
            m_jobContext.success(jobId);
        } catch (RuntimeException e) {
            m_jobContext.failure(jobId, null, e);
            // do not throw error, job queue will stop running.
            // error gets logged to job error table and sipxconfig.log
            LOG.error(e);
        }
    }

    public void generateProfilesAndRestart(Collection gateways) {
        for (Iterator i = gateways.iterator(); i.hasNext();) {
            Integer id = (Integer) i.next();
            generateProfileAndRestart(id);
        }
    }

    public void setGatewayContext(GatewayContext gatewayContext) {
        m_gatewayContext = gatewayContext;
    }

    public void setJobContext(JobContext jobContext) {
        m_jobContext = jobContext;
    }
}
