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
package org.sipfoundry.sipxconfig.site.phone;

import java.util.Collection;

import org.apache.tapestry.BaseComponent;
import org.apache.tapestry.IActionListener;
import org.apache.tapestry.IMarkupWriter;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.form.IPropertySelectionModel;
import org.sipfoundry.sipxconfig.components.TapestryContext;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.site.setting.BulkGroupAction;

public abstract class GroupActions extends BaseComponent {
    public abstract Collection getSelectedIds();

    public abstract IActionListener getSelectedAction();

    public abstract void setSelectedAction(IActionListener action);

    public abstract TapestryContext getTapestry();

    protected void renderComponent(IMarkupWriter writer, IRequestCycle cycle) {
        setSelectedAction(null);
        super.renderComponent(writer, cycle);
        if (TapestryUtils.isRewinding(cycle, this) && TapestryUtils.isValid(this)) {
            triggerAction(cycle);
        }
    }

    public IPropertySelectionModel decorateModel(IPropertySelectionModel model) {
        String label = getMessages().getMessage("label.moreActions");
        return getTapestry().addExtraOption(model, label);
    }

    private void triggerAction(IRequestCycle cycle) {
        IActionListener a = getSelectedAction();
        if (!(a instanceof BulkGroupAction)) {
            return;
        }
        Collection selectedIds = getSelectedIds();
        if (selectedIds != null && selectedIds.size() == 0) {
            // nothing to do
            return;
        }

        BulkGroupAction action = (BulkGroupAction) a;
        action.setIds(selectedIds);

        action.actionTriggered(this, cycle);

        String successMsg = action.getSuccessMsg(getContainer().getMessages());
        TapestryUtils.recordSuccess(this, successMsg);
    }
}
