/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.sipfoundry.sipxconfig.admin.dialplan.config.FullTransform;
import org.sipfoundry.sipxconfig.admin.dialplan.config.Transform;
import org.sipfoundry.sipxconfig.gateway.Gateway;
import org.sipfoundry.sipxconfig.permission.Permission;

/**
 * LongDistanceRule
 */
public class InternationalRule extends DialingRule {
    private String m_internationalPrefix;

    public InternationalRule() {
        // empty
    }

    public String[] getPatterns() {
        String pattern = m_internationalPrefix + "x.";
        return new String[] {
            pattern
        };
    }

    public List<Permission> getPermissions() {
        return Collections.singletonList(Permission.INTERNATIONAL_DIALING);
    }

    public Transform[] getTransforms() {
        CallPattern patternNormal = new CallPattern(m_internationalPrefix,
                CallDigits.VARIABLE_DIGITS);
        String user = patternNormal.calculatePattern();
        List<Gateway> gateways = getGateways();
        List<Transform> transforms = new ArrayList<Transform>(gateways.size());
        ForkQueueValue q = new ForkQueueValue(gateways.size());
        for (Gateway gateway : gateways) {
            FullTransform transform = new FullTransform();
            transform.setUser(gateway.getCallPattern(user));
            transform.setHost(gateway.getAddress());
            String[] fieldParams = new String[] {
                q.getSerial()
            };
            transform.setFieldParams(fieldParams);
            transforms.add(transform);
        }
        return transforms.toArray(new Transform[transforms.size()]);
    }

    public DialingRuleType getType() {
        return DialingRuleType.INTERNATIONAL;
    }

    /**
     * External rule - added to mappingrules.xml
     */
    public boolean isInternal() {
        return false;
    }

    public String getInternationalPrefix() {
        return m_internationalPrefix;
    }

    public void setInternationalPrefix(String internationalPrefix) {
        m_internationalPrefix = internationalPrefix;
    }
}
