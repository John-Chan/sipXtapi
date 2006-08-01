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
package org.sipfoundry.sipxconfig.setting;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import org.apache.commons.collections.Transformer;

/**
 * Common code for line, phone, and user information.
 */
public abstract class BeanWithGroups extends BeanWithSettings {
    public static final String GROUPS_PROP = "groups";

    private Set<Group> m_groups;

    @Override
    protected void initializeSettingModel() {
        setSettingModel2(new BeanWithGroupsModel(this));
    }

    public synchronized Set<Group> getGroups() {
        // lazy to avoid NPE in unit tests that create mock objects for subclasses
        if (m_groups == null) {
            setGroups(new TreeSet<Group>());
        }
        return m_groups;
    }

    public void setGroups(Set<Group> settingSets) {
        m_groups = settingSets;

        BeanWithGroupsModel model = (BeanWithGroupsModel) getSettingModel2();
        // passed collection is not copied
        model.setGroups(m_groups);
    }

    public List getGroupsAsList() {
        return new ArrayList(getGroups());
    }

    public void setGroupsAsList(List groups) {
        getGroups().clear();
        getGroups().addAll(groups);
    }

    public void addGroup(Group tag) {
        getGroups().add(tag);
    }

    public void removeGroup(Group tag) {
        getGroups().remove(tag);
    }

    public static class AddTag implements Transformer {
        private Group m_tag;

        public AddTag(Group tag) {
            m_tag = tag;
        }

        public Object transform(Object input) {
            BeanWithGroups bean = (BeanWithGroups) input;
            bean.addGroup(m_tag);
            return bean;
        }
    }

    public static class RemoveTag implements Transformer {
        private Group m_tag;

        public RemoveTag(Group tag) {
            m_tag = tag;
        }

        public Object transform(Object input) {
            BeanWithGroups bean = (BeanWithGroups) input;
            bean.removeGroup(m_tag);
            return bean;
        }
    }
}
