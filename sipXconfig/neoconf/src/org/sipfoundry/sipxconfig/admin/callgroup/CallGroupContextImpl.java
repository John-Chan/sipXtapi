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
package org.sipfoundry.sipxconfig.admin.callgroup;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import org.sipfoundry.sipxconfig.admin.ExtensionInUseException;
import org.sipfoundry.sipxconfig.admin.NameInUseException;
import org.sipfoundry.sipxconfig.admin.commserver.SipxReplicationContext;
import org.sipfoundry.sipxconfig.admin.commserver.imdb.DataSet;
import org.sipfoundry.sipxconfig.common.CollectionUtils;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.DaoUtils;
import org.sipfoundry.sipxconfig.common.SipxHibernateDaoSupport;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.common.event.UserDeleteListener;
import org.springframework.orm.hibernate3.HibernateTemplate;

/**
 * Hibernate implementation of the call group context
 */
public class CallGroupContextImpl extends SipxHibernateDaoSupport implements CallGroupContext {
    private static final String VALUE = "value";

    private static final String QUERY_CALL_GROUP_IDS_WITH_NAME = "callGroupIdsWithName";
    private static final String QUERY_CALL_GROUP_IDS_WITH_ALIAS = "callGroupIdsWithAlias";

    private CoreContext m_coreContext;
    private SipxReplicationContext m_replicationContext;

    // trivial setters
    public void setCoreContext(CoreContext coreContext) {
        m_coreContext = coreContext;
    }

    public void setReplicationContext(SipxReplicationContext replicationContext) {
        m_replicationContext = replicationContext;
    }

    public CallGroup loadCallGroup(Integer id) {
        return (CallGroup) getHibernateTemplate().load(CallGroup.class, id);
    }

    public void storeCallGroup(CallGroup callGroup) {
        // Check for duplicate names or extensions before saving the call group
        String name = callGroup.getName();
        final String huntGroupTypeName = "hunt group";
        DaoUtils.checkDuplicatesByNamedQuery(getHibernateTemplate(), callGroup,
                QUERY_CALL_GROUP_IDS_WITH_NAME, name, new NameInUseException(huntGroupTypeName,
                        name));
        String extension = callGroup.getExtension();
        DaoUtils.checkDuplicatesByNamedQuery(getHibernateTemplate(), callGroup,
                QUERY_CALL_GROUP_IDS_WITH_ALIAS, extension, new ExtensionInUseException(
                        "hunt group extension", extension));

        getHibernateTemplate().saveOrUpdate(callGroup);
        // activate call groups every time the call group is saved
        activateCallGroups();
    }

    public void removeCallGroups(Collection ids) {
        if (ids.isEmpty()) {
            return;
        }
        removeAll(CallGroup.class, ids);
        // activate call groups every time the call group is removed
        activateCallGroups();
    }

    public UserDeleteListener createUserDeleteListener() {
        return new OnUserDelete();
    }

    private class OnUserDelete extends UserDeleteListener {
        protected void onUserDelete(User user) {
            getHibernateTemplate().update(user);
            removeUser(user.getId());
        }
    }

    /**
     * Removes all rings associated with a removed user. This is temporary. In the long term we
     * will introduce hibernate dependency between the users and rings table.
     * 
     * Note: we cannot just blindly removed rings from database, there is a cascade relationship
     * between call groups and rings, hibernate will resave the ring if it's removed from database
     * but not from the call group
     * 
     * This function is called from legacy notification service, there is no need to activate call
     * groups - they will be activated anyway because alias generation follows user deletion.
     * 
     * @param userId id of the user that us being deleted
     * 
     */
    public void removeUser(Integer userId) {
        final HibernateTemplate hibernate = getHibernateTemplate();
        Collection rings = hibernate.findByNamedQueryAndNamedParam("userRingsForUserId",
                "userId", userId);
        for (Iterator i = rings.iterator(); i.hasNext();) {
            UserRing ring = (UserRing) i.next();
            CallGroup callGroup = ring.getCallGroup();
            callGroup.removeRing(ring);
            hibernate.save(callGroup);
        }
    }

    public List getCallGroups() {
        return getHibernateTemplate().loadAll(CallGroup.class);
    }

    public void duplicateCallGroups(Collection ids) {
        for (Iterator i = ids.iterator(); i.hasNext();) {
            CallGroup group = loadCallGroup((Integer) i.next());

            // Create a copy of the call group with a unique name
            CallGroup groupDup = (CallGroup) duplicateBean(group, QUERY_CALL_GROUP_IDS_WITH_NAME);

            // Extensions should be unique, so don't copy the extension from the
            // source call group. The admin must fill it in explicitly.
            groupDup.setExtension(null);

            groupDup.setEnabled(false);
            storeCallGroup(groupDup);
        }
    }

    /**
     * Remove all call groups - mostly used for testing
     */
    public void clear() {
        HibernateTemplate template = getHibernateTemplate();
        Collection callGroups = template.loadAll(CallGroup.class);
        template.deleteAll(callGroups);
    }

    /**
     * Sends notification to profile generator to trigger alias generation
     */
    public void activateCallGroups() {
        m_replicationContext.generate(DataSet.ALIAS);
    }

    /**
     * Generate aliases for all call groups
     */
    public Collection getAliasMappings() {
        final String dnsDomain = m_coreContext.getDomainName();
        Collection callGroups = getCallGroups();
        List allAliases = new ArrayList(callGroups.size());
        for (Iterator i = callGroups.iterator(); i.hasNext();) {
            CallGroup cg = (CallGroup) i.next();
            allAliases.addAll(cg.generateAliases(dnsDomain));
        }
        return allAliases;
    }

    public boolean isAliasInUse(String alias) {
        // Look for the ID of a call group with the specified alias as its name or extension.
        // If there is one, then the alias is in use.
        List objs = getHibernateTemplate().findByNamedQueryAndNamedParam(
                QUERY_CALL_GROUP_IDS_WITH_ALIAS, VALUE, alias);
        return CollectionUtils.safeSize(objs) > 0;
    }

    public void addUsersToCallGroup(Integer callGroupId, Collection ids) {
        CallGroup callGroup = loadCallGroup(callGroupId);
        for (Iterator i = ids.iterator(); i.hasNext();) {
            Integer userId = (Integer) i.next();
            User user = m_coreContext.loadUser(userId);
            callGroup.insertRing(user);
        }
        storeCallGroup(callGroup);
    }
}
