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
package org.sipfoundry.sipxconfig.api;

import java.util.Arrays;
import java.util.Collection;
import java.util.Set;

import org.apache.commons.collections.CollectionUtils;
import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.common.NamedObject;
import org.sipfoundry.sipxconfig.setting.BeanWithGroups;

public class UserBuilder extends SimpleBeanBuilder {
    private static final String ALIASES_PROP = org.sipfoundry.sipxconfig.common.User.ALIASES_PROP;
    private static final String GROUPS_PROP = BeanWithGroups.GROUPS_PROP;

    private static final String[] IGNORE_LIST = {
        ALIASES_PROP, GROUPS_PROP
    };

    public UserBuilder() {
        getCustomFields().addAll(Arrays.asList(IGNORE_LIST));
    }

    public void toApiObject(Object apiObject, Object myObject, Set properties) {
        super.toApiObject(apiObject, myObject, properties);
        org.sipfoundry.sipxconfig.common.User my = (org.sipfoundry.sipxconfig.common.User) myObject;
        User api = (User) apiObject;
        if (properties.contains(ALIASES_PROP) && !StringUtils.isBlank(my.getAliasesString())) {
            api.setAliases((String[]) my.getAliases().toArray(new String[0]));
        }
        if (properties.contains(GROUPS_PROP)) {
            Collection groupNames = CollectionUtils.collect(my.getGroups(),
                    new NamedObject.ToName());
            api.setGroups((String[]) groupNames.toArray(new String[groupNames.size()]));
        }
    }

    public void toMyObject(Object myObject, Object apiObject, Set properties) {
        super.toMyObject(myObject, apiObject, properties);
        org.sipfoundry.sipxconfig.common.User my = (org.sipfoundry.sipxconfig.common.User) myObject;
        User api = (User) apiObject;
        if (properties.contains(ALIASES_PROP) && api.getAliases() != null) {
            my.getAliases().clear();
            my.addAliases(api.getAliases());
        }
    }
}
