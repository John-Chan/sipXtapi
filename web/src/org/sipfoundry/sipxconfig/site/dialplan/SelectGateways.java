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
package org.sipfoundry.sipxconfig.site.dialplan;

import java.util.Collection;
import java.util.Iterator;

import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.html.BasePage;

import org.sipfoundry.sipxconfig.admin.dialplan.DialPlanManager;
import org.sipfoundry.sipxconfig.admin.dialplan.DialingRule;
import org.sipfoundry.sipxconfig.admin.dialplan.Gateway;

/**
 * List all the gateways, allow adding and deleting gateways
 */
public abstract class SelectGateways extends BasePage implements PageRenderListener {
    public static final String PAGE = "SelectGateways";

    // virtual properties
    public abstract DialPlanManager getDialPlanManager();

    public abstract Integer getRuleId();

    public abstract void setRuleId(Integer id);

    public abstract Collection getSelectedRows();

    public abstract void setGateways(Collection gateways);

    public abstract Collection getGateways();
    
    public abstract String getNextPage();
    
    public abstract void setNextPage(String nextPage);

    public void pageBeginRender(PageEvent event_) {
        Collection gateways = getGateways();
        if (null == gateways) {
            gateways = getDialPlanManager().getAvailableGateways(getRuleId());
            setGateways(gateways);
        }
    }

    public void formSubmit(IRequestCycle cycle) {
        Collection selectedRows = getSelectedRows();
        if (selectedRows != null) {
            selectGateways(selectedRows);
        }
        EditDialRule editPage = (EditDialRule) cycle.getPage(getNextPage());
        editPage.setRuleId(getRuleId());
        cycle.activate(editPage);
    }

    /**
     * Adds/removes gateways from dial plan
     * 
     * @param gateways list of gateway ids to be added to the dial plan
     * @param emergency true for emergency gateways
     */
    private void selectGateways(Collection gateways) {
        //FIXME: selecting gateways does not work here: rules and gateways are detached by now
        DialPlanManager manager = getDialPlanManager();
        DialingRule rule = manager.getFlexDialPlan().getRule(getRuleId());
        if (null != rule) {
            for (Iterator i = gateways.iterator(); i.hasNext();) {
                Integer id = (Integer) i.next();
                Gateway gateway = manager.getGateway(id);
                if (null != gateway) {
                    rule.addGateway(gateway);
                }
            }
        }
    }
}
