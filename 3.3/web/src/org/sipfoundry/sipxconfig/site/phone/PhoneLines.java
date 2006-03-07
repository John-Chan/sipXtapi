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
package org.sipfoundry.sipxconfig.site.phone;

import java.util.Collection;

import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.callback.PageCallback;
import org.apache.tapestry.event.PageBeginRenderListener;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.common.DataCollectionUtil;
import org.sipfoundry.sipxconfig.components.SelectMap;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.PhoneContext;
import org.sipfoundry.sipxconfig.site.line.AddExternalLine;
import org.sipfoundry.sipxconfig.site.line.EditLine;

/**
 * Manage a phone's lines
 */
public abstract class PhoneLines extends BasePage implements PageBeginRenderListener {

    public static final String PAGE = "PhoneLines";

    public abstract Phone getPhone();

    public abstract void setPhone(Phone phone);

    public abstract Integer getPhoneId();

    /** REQUIRED PROPERTY */
    public abstract void setPhoneId(Integer id);

    public Collection getLines() {
        return getPhone().getLines();
    }

    public abstract Line getCurrentRow();

    public abstract void setCurrentRow(Line line);

    public abstract SelectMap getSelections();

    public abstract void setSelections(SelectMap selections);

    public abstract PhoneContext getPhoneContext();

    public void pageBeginRender(PageEvent event_) {
        PhoneContext context = getPhoneContext();
        Phone phone = context.loadPhone(getPhoneId());
        setPhone(phone);

        // Generate the list of phone items
        if (getSelections() == null) {
            setSelections(new SelectMap());
        }
    }

    public void addExternalLine(IRequestCycle cycle) {
        Object[] params = cycle.getListenerParameters();
        Integer phoneId = (Integer) TapestryUtils.assertParameter(Integer.class, params, 0);
        checkMaxLines(phoneId);
        AddExternalLine page = (AddExternalLine) cycle.getPage(AddExternalLine.PAGE);
        page.setPhoneId(phoneId);
        page.setCallback(new PageCallback(this));
        cycle.activate(page);
    }

    private void checkMaxLines(Integer phoneId) {
        Phone phone = getPhoneContext().loadPhone(phoneId);
        phone.addLine(phone.createLine());
    }

    public void addLine(IRequestCycle cycle) {
        Object[] params = cycle.getListenerParameters();
        Integer phoneId = (Integer) TapestryUtils.assertParameter(Integer.class, params, 0);
        checkMaxLines(phoneId);
        AddPhoneUser page = (AddPhoneUser) cycle.getPage(AddPhoneUser.PAGE);
        page.setPhoneId(phoneId);
        cycle.activate(page);
    }

    public void editLine(IRequestCycle cycle) {
        Object[] params = cycle.getListenerParameters();
        Integer lineId = (Integer) TapestryUtils.assertParameter(Integer.class, params, 0);
        EditLine page = (EditLine) cycle.getPage(EditLine.PAGE);
        page.setLineId(lineId);
        cycle.activate(page);
    }

    public void deleteLine() {
        PhoneContext context = getPhoneContext();
        Phone phone = getPhone();

        // hack, avoid hibernate exception on unsaved valuestorage objects
        // on orphaned lines
        context.storePhone(phone);

        Object[] lineIds = getSelections().getAllSelected().toArray();
        DataCollectionUtil.removeByPrimaryKey(phone.getLines(), lineIds);

        context.storePhone(phone);
    }

    public void moveLineUp() {
        moveLines(-1);
    }

    public void moveLineDown() {
        moveLines(1);
    }

    private void moveLines(int step) {
        PhoneContext context = getPhoneContext();
        Phone phone = getPhone();
        Object[] lineIds = getSelections().getAllSelected().toArray();
        DataCollectionUtil.moveByPrimaryKey(phone.getLines(), lineIds, step);
        context.storePhone(phone);
    }

    public String ok() {
        apply();
        return ManagePhones.PAGE;
    }

    public void apply() {
        PhoneContext dao = getPhoneContext();
        dao.storePhone(getPhone());
        dao.flush();
    }

    public String cancel() {
        return ManagePhones.PAGE;
    }
}
