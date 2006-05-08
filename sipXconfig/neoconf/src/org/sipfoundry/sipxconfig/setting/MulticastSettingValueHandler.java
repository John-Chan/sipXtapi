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
package org.sipfoundry.sipxconfig.setting;

import java.util.Collection;

public class MulticastSettingValueHandler implements SettingValueHandler {
    private Collection<SettingValueHandler> m_resolvers;
    
    public MulticastSettingValueHandler(Collection<SettingValueHandler> resolvers) {
        m_resolvers = resolvers;
    }

    public SettingValue2 getSettingValue(String path) {
        SettingValue2 value = null;
        for (SettingValueHandler resolver : m_resolvers) {
            SettingValue2 sv = resolver.getSettingValue(path);
            if (sv != null) {
                value = sv;
            }
        }

        return value;
    }

}
