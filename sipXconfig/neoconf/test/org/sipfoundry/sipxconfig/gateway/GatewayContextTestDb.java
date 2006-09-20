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
package org.sipfoundry.sipxconfig.gateway;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import org.dbunit.dataset.ITable;
import org.sipfoundry.sipxconfig.SipxDatabaseTestCase;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.admin.dialplan.DialPlanContext;
import org.sipfoundry.sipxconfig.admin.dialplan.EmergencyRouting;
import org.sipfoundry.sipxconfig.admin.dialplan.InternationalRule;
import org.sipfoundry.sipxconfig.phone.PhoneModel;
import org.springframework.context.ApplicationContext;

public class GatewayContextTestDb extends SipxDatabaseTestCase {

    private GatewayContext m_context;

    private DialPlanContext m_dialPlanContext;

    private ApplicationContext m_appContext;

    protected void setUp() throws Exception {
        m_appContext = TestHelper.getApplicationContext();
        m_context = (GatewayContext) m_appContext.getBean(GatewayContext.CONTEXT_BEAN_NAME);
        m_dialPlanContext = (DialPlanContext) m_appContext
                .getBean(DialPlanContext.CONTEXT_BEAN_NAME);
        TestHelper.cleanInsert("ClearDb.xml");
    }

    public void testAddGateway() {
        Gateway g1 = new Gateway();
        Gateway g2 = new Gateway();

        // add g1
        m_context.storeGateway(g1);

        assertEquals(1, m_context.getGateways().size());
        assertTrue(m_context.getGateways().contains(g1));

        // add g2
        m_context.storeGateway(g2);

        assertEquals(2, m_context.getGateways().size());
        assertTrue(m_context.getGateways().contains(g1));
        assertTrue(m_context.getGateways().contains(g2));
    }

    public void testDeleteGateway() {
        Gateway g1 = new Gateway();
        Gateway g2 = new Gateway();
        Gateway g3 = new Gateway();

        // add all
        m_context.storeGateway(g1);
        m_context.storeGateway(g2);
        m_context.storeGateway(g3);

        Integer[] toBeRemoved = {
            g1.getId(), g3.getId()
        };
        m_context.deleteGateways(Arrays.asList(toBeRemoved));

        List gateways = m_context.getGateways();

        assertEquals(1, gateways.size());
        assertFalse(gateways.contains(g1));
        assertTrue(gateways.contains(g2));
        assertFalse(gateways.contains(g3));
    }

    public void testUpdateGateway() throws Exception {
        Gateway g1 = new Gateway();
        g1.setAddress("10.1.1.1");
        m_context.storeGateway(g1);
        g1.setAddress("10.1.1.2");
        g1.setPrefix("33");

        GatewayCallerAliasInfo info = new GatewayCallerAliasInfo();
        info.setDefaultCallerAlias("3211231234");
        info.setAnonymous(true);
        info.setKeepDigits(10);
        info.setIgnoreUserInfo(true);
        info.setTransformUserExtension(true);
        info.setAddPrefix("4321");
        g1.setCallerAliasInfo(info);

        m_context.storeGateway(g1);
        assertEquals("10.1.1.2", g1.getAddress());
        assertEquals("33", g1.getPrefix());
        assertEquals("3211231234", g1.getCallerAliasInfo().getDefaultCallerAlias());
        assertEquals("4321", g1.getCallerAliasInfo().getAddPrefix());
        assertEquals(10, g1.getCallerAliasInfo().getKeepDigits());
        assertTrue(g1.getCallerAliasInfo().isAnonymous());
        assertTrue(g1.getCallerAliasInfo().isIgnoreUserInfo());
        assertTrue(g1.getCallerAliasInfo().isTransformUserExtension());
    }

    public void testSaveLoadUpdateGateway() throws Exception {
        Gateway g1 = new Gateway();
        g1.setAddress("10.1.1.1");
        m_context.storeGateway(g1);

        Gateway g2 = m_context.getGateway(g1.getId());
        g2.setAddress("10.1.1.2");
        m_context.storeGateway(g2);
        assertEquals("10.1.1.2", g2.getAddress());
    }

    public void testDeleteGatewayInUse() {
        Gateway g1 = new Gateway();
        g1.setAddress("10.1.1.1");
        m_context.storeGateway(g1);
        InternationalRule rule = new InternationalRule();
        rule.setName("testRule");
        rule.setInternationalPrefix("011");
        rule.addGateway(g1);

        m_dialPlanContext.storeRule(rule);
        // remove gateway
        m_context.deleteGateways(Collections.singletonList(g1.getId()));

        Integer ruleId = rule.getId();

        rule = (InternationalRule) m_dialPlanContext.getRule(ruleId);
        assertTrue(rule.getGateways().isEmpty());
    }

    public void testDeleteGatewayInUseByEmergencyRouting() {
        Gateway g1 = new Gateway();
        g1.setAddress("10.1.1.1");
        m_context.storeGateway(g1);

        EmergencyRouting emergencyRouting = m_dialPlanContext.getEmergencyRouting();
        emergencyRouting.setDefaultGateway(g1);
        m_dialPlanContext.storeEmergencyRouting(emergencyRouting);

        // remove gateway
        m_context.deleteGateways(Collections.singletonList(g1.getId()));

        emergencyRouting = m_dialPlanContext.getEmergencyRouting();
        assertNull(emergencyRouting.getDefaultGateway());
    }

    public void testAllGateways() throws Exception {
        Collection models = m_context.getAvailableGatewayModels();
        for (Iterator i = models.iterator(); i.hasNext();) {
            PhoneModel model = (PhoneModel) i.next();
            Gateway gateway = m_context.newGateway(model);
            String beanId = model.getBeanId();
            assertEquals(gateway.getClass(), m_appContext.getBean(beanId).getClass());
            if (beanId.equals("gwGeneric") || beanId.equals("gwSipTrunk")) {
                assertNull(gateway.getSettings());
            } else {
                assertNotNull(gateway.getSettings());
            }
            m_context.storeGateway(gateway);
        }
        ITable actual = TestHelper.getConnection().createDataSet().getTable("gateway");
        // one gateway per row
        assertEquals(models.size(), actual.getRowCount());
    }

    public void testGetGatewaySettings() throws Exception {
        TestHelper.cleanInsertFlat("gateway/seed_gateway.db.xml");
        Gateway gateway = m_context.getGateway(new Integer(1001));
        assertNotNull(gateway.getTftpRoot());
    }
}
