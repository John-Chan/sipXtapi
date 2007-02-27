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
package org.sipfoundry.sipxconfig.admin.commserver;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.sipfoundry.sipxconfig.admin.commserver.imdb.DataSet;
import org.sipfoundry.sipxconfig.admin.dialplan.config.XmlFile;
import org.sipfoundry.sipxconfig.common.LazyDaemon;
import org.springframework.context.ApplicationEvent;

public class LazySipxReplicationContextImpl implements SipxReplicationContext {
    private static final Log LOG = LogFactory.getLog(LazySipxReplicationContextImpl.class);
    /**
     * 7s is the default sleep interval after any replication request is issued
     */
    private static final int DEFAULT_SLEEP_INTERVAL = 7000;

    private Set m_tasks = new HashSet();

    private List m_events = new ArrayList();

    private SipxReplicationContext m_target;

    private int m_sleepInterval = DEFAULT_SLEEP_INTERVAL;

    /**
     * Do not initialized worker here - properties must be set first
     */
    private Worker m_worker;

    public void init() {
        m_worker = new Worker();
        m_worker.start();
    }

    public synchronized void generate(DataSet dataSet) {
        m_tasks.add(dataSet);
        notifyWorker();
    }

    public synchronized void generateAll() {
        m_tasks.addAll(DataSet.getEnumList());
        notifyWorker();
    }

    public synchronized void replicate(XmlFile xmlFile) {
        m_tasks.add(xmlFile);
        notifyWorker();
    }

    public String getXml(DataSet dataSet) {
        return m_target.getXml(dataSet);
    }
    
    public synchronized void publishEvent(ApplicationEvent event) {
        m_events.add(event);
        // we call notify and not notifyWorker - publishing event is not real work
        LOG.debug("Notify work on published event");
        notify();
    }

    private void notifyWorker() {
        m_worker.workScheduled();
        LOG.debug("Notify work scheduled");
        notify();
    }

    private synchronized void waitForWork() throws InterruptedException {
        if (m_tasks.isEmpty()) {
            LOG.debug("Waiting for work");
            wait();
            LOG.debug("Work received");
        }
    }

    private void publishQueuedEvents() {
        for (Iterator u = m_events.iterator(); u.hasNext();) {
            ApplicationEvent event = (ApplicationEvent) u.next();
            m_target.publishEvent(event);
        }
        m_events.clear();
    }

    private synchronized Set getTasks() {
        if (m_tasks.isEmpty()) {
            return Collections.EMPTY_SET;
        }
        Set oldTasks = m_tasks;
        m_tasks = new HashSet();
        return oldTasks;
    }

    public void setTarget(SipxReplicationContext target) {
        m_target = target;
    }

    public void setSleepInterval(int sleepInterval) {
        m_sleepInterval = sleepInterval;
    }

    /**
     * Worker thread: waits till someone needs to be done, sleeps for a bit and does it.
     */
    private class Worker extends LazyDaemon {
        public Worker() {
            super("Replication worker thread", m_sleepInterval);
        }

        protected void waitForWork() throws InterruptedException {
            LazySipxReplicationContextImpl.this.waitForWork();
        }

        protected boolean work() {
            Set tasks = getTasks();
            for (Iterator i = tasks.iterator(); i.hasNext();) {
                Object next = i.next();
                if (next instanceof DataSet) {
                    DataSet ds = (DataSet) next;
                    m_target.generate(ds);
                }
                if (next instanceof XmlFile) {
                    XmlFile file = (XmlFile) next;
                    m_target.replicate(file);
                }
            }
            // before we start waiting publish all the events that are in the queue
            publishQueuedEvents();
            return true;
        }
    }
}