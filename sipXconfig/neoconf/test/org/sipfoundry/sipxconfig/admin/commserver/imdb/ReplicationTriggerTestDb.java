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
package org.sipfoundry.sipxconfig.admin.commserver.imdb;

import org.easymock.EasyMock;
import org.easymock.IMocksControl;
import org.sipfoundry.sipxconfig.SipxDatabaseTestCase;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.admin.commserver.SipxReplicationContext;
import org.sipfoundry.sipxconfig.admin.parkorbit.ParkOrbitContext;
import org.sipfoundry.sipxconfig.common.ApplicationInitializedEvent;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.setting.Group;
import org.sipfoundry.sipxconfig.setting.SettingDao;
import org.springframework.context.ApplicationContext;

public class ReplicationTriggerTestDb extends SipxDatabaseTestCase {

    private ReplicationTrigger m_trigger;
    private SipxReplicationContext m_oldReplicationContext;
    private IMocksControl m_replicationContextCtrl;
    private SipxReplicationContext m_replicationContext;
    private SettingDao m_dao;
    private IMocksControl m_parkOrbitsContextCtrl;
    private ParkOrbitContext m_parkOrbitsContext;
    private ParkOrbitContext m_oldParkOrbitContext;

    protected void setUp() throws Exception {
        ApplicationContext app = TestHelper.getApplicationContext();
        m_trigger = (ReplicationTrigger) app.getBean("replicationTrigger");
        m_oldParkOrbitContext = m_trigger.getParkOrbitContext();
        m_oldReplicationContext = m_trigger.getReplicationContext();

        TestHelper.cleanInsert("ClearDb.xml");

        m_replicationContextCtrl = EasyMock.createStrictControl();
        m_replicationContext = m_replicationContextCtrl.createMock(SipxReplicationContext.class);

        m_parkOrbitsContextCtrl = EasyMock.createControl();
        m_parkOrbitsContext = m_parkOrbitsContextCtrl.createMock(ParkOrbitContext.class);

        m_dao = (SettingDao) app.getBean("settingDao");
    }

    protected void tearDown() {
        m_trigger.setReplicationContext(m_oldReplicationContext);
        m_trigger.setParkOrbitContext(m_oldParkOrbitContext);
        m_parkOrbitsContextCtrl.verify();
        m_replicationContextCtrl.verify();
    }

    /**
     * Call this method to set up the mock control to expect exactly one call to
     * m_replicationContext.generate .
     */
    private void expectOneCallToGenerate() {
        m_replicationContext.generate(DataSet.PERMISSION);
        m_replicationContextCtrl.replay();
        m_parkOrbitsContextCtrl.replay();
    }

    /**
     * Call this method to set up the mock control to expect exactly one call to
     * m_replicationContext.generateAll .
     */
    private void expectOneCallToGenerateAll() {
        m_replicationContext.generateAll();
        m_replicationContextCtrl.replay();
        m_parkOrbitsContext.activateParkOrbits();
        m_parkOrbitsContextCtrl.replay();
    }

    /**
     * Call this method to set up the mock control to expect no calls.
     */
    private void expectNoCalls() {
        m_replicationContextCtrl.replay();
        m_parkOrbitsContextCtrl.replay();
    }

    /**
     * Test that saving a user group in db actually triggers a replication
     */
    public void testNewUserGroup() throws Exception {
        m_trigger.setReplicationContext(m_replicationContext);
        Group g = new Group();
        g.setName("replicationTriggerTest");
        g.setResource(User.GROUP_RESOURCE_ID);
        expectOneCallToGenerate();
        m_dao.saveGroup(g);
    }

    public void testUpdateUserGroup() throws Exception {
        TestHelper.cleanInsertFlat("admin/commserver/imdb/UserGroupSeed.db.xml");
        m_trigger.setReplicationContext(m_replicationContext);
        Group g = m_dao.getGroup(new Integer(1000));
        expectOneCallToGenerate();
        m_dao.saveGroup(g);
    }

    /**
     * Test that replication happens at app startup if the replicateOnStartup property is set
     */
    public void testReplicateOnStartup() throws Exception {
        m_trigger.setReplicationContext(m_replicationContext);
        m_trigger.setParkOrbitContext(m_parkOrbitsContext);
        m_trigger.setReplicateOnStartup(true);
        expectOneCallToGenerateAll();
        m_trigger.onApplicationEvent(new ApplicationInitializedEvent(new Object()));
    }

    /**
     * Test that replication doesn't happen at app startup if the replicateOnStartup property is
     * off
     */
    public void testNoReplicateOnStartup() throws Exception {
        m_trigger.setReplicationContext(m_replicationContext);
        m_trigger.setReplicateOnStartup(false);
        expectNoCalls();
        m_trigger.onApplicationEvent(new ApplicationInitializedEvent(new Object()));
    }

}
