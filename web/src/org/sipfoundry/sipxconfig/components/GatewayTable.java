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
package org.sipfoundry.sipxconfig.components;

import org.apache.tapestry.BaseComponent;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.sipfoundry.sipxconfig.admin.dialplan.Gateway;
import org.sipfoundry.sipxconfig.site.dialplan.EditGateway;

/**
 * GatewayTable
 */
public abstract class GatewayTable extends BaseComponent implements PageRenderListener {
    public abstract Gateway getCurrentRow();

    public abstract String getEditPageName();

    public abstract Integer getRuleId();

    public abstract SelectMap getSelections();

    public abstract void setSelections(SelectMap selected);

    public void pageBeginRender(PageEvent event_) {
        SelectMap selections = getSelections();
        if (null == selections) {
            setSelections(new SelectMap());
        }
    }

    /**
     * When user clicks on link to edit a gateway
     */
    public void edit(IRequestCycle cycle) {
        String editPageName = getEditPageName();
        EditGateway page = (EditGateway) cycle.getPage(editPageName);

        Gateway currentRow = getCurrentRow();
        page.setGatewayId(currentRow.getId());
        page.setRuleId(getRuleId());
        page.setNextPage(cycle.getPage().getPageName());        
        cycle.activate(page);
    }
}
