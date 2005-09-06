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
package org.sipfoundry.sipxconfig.admin.forwarding;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import org.sipfoundry.sipxconfig.admin.commserver.SipxReplicationContext;
import org.sipfoundry.sipxconfig.admin.commserver.imdb.DataSet;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.Permission;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.common.event.UserDeleteListener;
import org.springframework.orm.hibernate3.HibernateTemplate;
import org.springframework.orm.hibernate3.support.HibernateDaoSupport;

/**
 * ForwardingContextImpl
 * 
 */
public class ForwardingContextImpl extends HibernateDaoSupport implements ForwardingContext {
    private CoreContext m_coreContext;

    private SipxReplicationContext m_replicationContext;

    /**
     * Looks for a call sequence associated with a given user.
     * 
     * This version just assumes that CallSequence id is the same as user id. More general
     * implementation would run a query. <code>
     *      String ringsForUser = "from CallSequence cs where cs.user = :user";
     *      hibernate.findByNamedParam(ringsForUser, "user", user);
     * </code>
     * 
     * @param user for which CallSequence object is retrieved
     */
    public CallSequence getCallSequenceForUser(User user) {
        return getCallSequenceForUserId(user.getId());
    }

    public void saveCallSequence(CallSequence callSequence) {
        saveCallSequence(callSequence, false);
    }

    private void saveCallSequence(CallSequence callSequence, boolean notify) {
        getHibernateTemplate().update(callSequence);
        if (notify) {
            // Notify commserver of ALIAS and AUTH_EXCEPTIONS
            m_replicationContext.generate(DataSet.ALIAS);
            m_replicationContext.generate(DataSet.AUTH_EXCEPTION);
        }
    }

    public void flush() {
        getHibernateTemplate().flush();
    }

    public CallSequence getCallSequenceForUserId(Integer userId) {
        HibernateTemplate hibernate = getHibernateTemplate();
        return (CallSequence) hibernate.load(CallSequence.class, userId);
    }

    public void removeCallSequenceForUserId(Integer userId, boolean notify) {
        CallSequence callSequence = getCallSequenceForUserId(userId);
        callSequence.clear();
        saveCallSequence(callSequence, notify);
    }

    public Ring getRing(Integer id) {
        HibernateTemplate hibernate = getHibernateTemplate();
        return (Ring) hibernate.load(Ring.class, id);
    }

    public Collection getAliasMappings() {
        List aliases = new ArrayList();
        List sequences = loadAllCallSequences();
        for (Iterator i = sequences.iterator(); i.hasNext();) {
            CallSequence sequence = (CallSequence) i.next();
            aliases.addAll(sequence.generateAliases(m_coreContext.getDomainName()));
        }
        return aliases;
    }

    public List getForwardingAuthExceptions() {
        List aliases = new ArrayList();
        List sequences = loadAllCallSequences();
        for (Iterator i = sequences.iterator(); i.hasNext();) {
            CallSequence sequence = (CallSequence) i.next();
            User user = sequence.getUser();
            if (user.hasPermission(Permission.FORWARD_CALLS_EXTERNAL)) {
                aliases.addAll(sequence.generateAuthExceptions());
            }
        }

        return aliases;
    }

    /**
     * Loads call sequences for all uses in current root organization
     * 
     * @return list of CallSequence objects
     */
    private List loadAllCallSequences() {
        List sequences = getHibernateTemplate().find("from CallSequence cs");
        return sequences;
    }

    public void setCoreContext(CoreContext coreContext) {
        m_coreContext = coreContext;
    }

    public void setReplicationContext(SipxReplicationContext replicationContext) {
        m_replicationContext = replicationContext;
    }

    public UserDeleteListener createUserDeleteListener() {
        return new OnUserDelete();
    }

    private class OnUserDelete extends UserDeleteListener {
        protected void onUserDelete(User user) {
            // false means - do not replicate - since we are deleting the user it'll happen
            // automatically
            removeCallSequenceForUserId(user.getId(), false);
        }
    }
}
