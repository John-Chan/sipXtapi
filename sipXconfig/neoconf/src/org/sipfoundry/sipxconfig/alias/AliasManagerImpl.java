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
package org.sipfoundry.sipxconfig.alias;

import java.lang.reflect.Proxy;
import java.net.URL;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.sipfoundry.sipxconfig.common.BeanId;
import org.sipfoundry.sipxconfig.common.BeanWithId;
import org.sipfoundry.sipxconfig.common.SipxHibernateDaoSupport;
import org.springframework.beans.factory.BeanInitializationException;
import org.springframework.beans.factory.ListableBeanFactory;
import org.springframework.context.ApplicationEvent;
import org.springframework.context.ApplicationListener;
import org.springframework.context.event.ContextRefreshedEvent;

// DO_NEXT: Move extension-related classes into this package and move any bean definitions into
// this beans.xml.

/**
 * AliasManagerImpl: manages all SIP aliases
 */
public class AliasManagerImpl extends SipxHibernateDaoSupport implements AliasManager,
        ApplicationListener {
    public static final String CONTEXT_BEAN_NAME = "aliasManagerImpl";
    private static final Log LOG = LogFactory.getLog(AliasManagerImpl.class);
    private Collection m_aliasOwners;
    private ListableBeanFactory m_beanFactory;

    /** Return true if the alias is in use by some AliasOwner */
    public boolean isAliasInUse(String alias) {
        for (Iterator iter = getAliasOwners().iterator(); iter.hasNext();) {
            AliasOwner owner = (AliasOwner) iter.next();
            if (owner.isAliasInUse(alias)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Return the bean IDs of objects with this alias by asking all the AliasOwners. Return null
     * if there is no such object.
     */
    public Collection getBeanIdsOfObjectsWithAlias(String alias) {
        Collection objects = new ArrayList();
        for (Iterator iter = getAliasOwners().iterator(); iter.hasNext();) {
            AliasOwner owner = (AliasOwner) iter.next();
            objects.addAll(owner.getBeanIdsOfObjectsWithAlias(alias));
        }
        return objects;
    }

    public boolean canObjectUseAlias(BeanWithId bean, String alias) {
        if (alias == null) {
            return true;
        }
        if (bean.isNew()) {
            // For a new bean, if there is any database object that has already claimed
            // the alias, then this bean cannot use it
            return !isAliasInUse(alias);
        }

        boolean canUseAlias = false;
        Collection bids = getBeanIdsOfObjectsWithAlias(alias);
        int size = bids.size();
        if (size == 0) {
            // No one is using the alias so we can take it
            canUseAlias = true;
        } else if (size == 1) {
            // One object is using the alias. If that object is the bean itself, then
            // it's OK for the bean to use the alias, otherwise not.
            BeanId bid = (BeanId) bids.iterator().next();
            canUseAlias = bid.isIdOfBean(bean);
            if (LOG.isInfoEnabled() && !canUseAlias) {
                String msg = MessageFormat.format("Alias \"{0}\" is in use by: {1}",
                        new Object[] {
                            alias, bid.toString()
                        });
                LOG.info(msg);
            }
        } else {
            // More than one object is using the alias! If the bean is one of them,
            // then the bean can continue to use the alias, but log an error in any
            // case because the database is messed up.
            for (Iterator iter = bids.iterator(); iter.hasNext();) {
                BeanId bid = (BeanId) iter.next();
                if (bid.isIdOfBean(bean)) {
                    canUseAlias = true;
                    break;
                }
            }
            if (LOG.isErrorEnabled()) {
                String msg = MessageFormat.format("Alias \"{0}\" is in use multiple objects:",
                        new Object[] {
                            alias
                        });
                LOG.error(msg + StringUtils.join(bids.iterator(), ' '));
            }
        }
        return canUseAlias;
    }

    /** Return all alias owners (excluding the AliasManagerImpl itself) */
    protected Collection getAliasOwners() {
        if (m_aliasOwners == null) { // lazy initialization
            if (m_beanFactory == null) {
                throw new BeanInitializationException(getClass().getName() + " not initialized");
            }
            Map beanMap = m_beanFactory.getBeansOfType(AliasOwner.class, false, true);
            m_aliasOwners = new ArrayList(beanMap.size());
            // Collect all proxies
            for (Iterator i = beanMap.values().iterator(); i.hasNext();) {
                AliasOwner owner = (AliasOwner) i.next();
                // Only include beans created through Factories - need Hibernate support.
                // Exclude the AliasManagerImpl itself, or we'll get into infinite recursion
                // when calling the alias owners.
                if (owner instanceof Proxy && !(owner instanceof AliasManager)) {
                    m_aliasOwners.add(owner);
                }
            }

        }
        return m_aliasOwners;
    }

    /**
     * Updates reference to bean factory and cleans the cache of alias owners
     */
    public void onApplicationEvent(ApplicationEvent event) {
        if (event instanceof ContextRefreshedEvent) {
            ContextRefreshedEvent cre = (ContextRefreshedEvent) event;
            m_beanFactory = cre.getApplicationContext();
            m_aliasOwners = null;

            // For debugging purposes, print out the location of the file log4j.properties
            // on the classpath, if it is there. No particular reason for this code to be
            // here, but it needed to go somewhere.
            // Comment out the println call by default so checkstyle won't complain about the use
            // of System.out.
            URL url = getClass().getResource("log4j.properties");
            if (url != null) {
//                System.out.println("Found log4j.properties: " + url);
                url.getClass();     // noop line of code to keep checkstyle from complaining about empty "if"
            }
        }
    }
}
