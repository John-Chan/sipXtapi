/*
 * 
 * 
 * Copyright (C) 2006 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2006 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.site.phone;

import org.apache.tapestry.BaseComponent;
import org.apache.tapestry.IMarkupWriter;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.annotations.ComponentClass;
import org.apache.tapestry.annotations.InitialValue;
import org.apache.tapestry.annotations.InjectObject;
import org.apache.tapestry.annotations.InjectPage;
import org.apache.tapestry.annotations.Message;
import org.apache.tapestry.annotations.Parameter;
import org.apache.tapestry.form.IPropertySelectionModel;
import org.sipfoundry.sipxconfig.components.ExtraOptionModelDecorator;
import org.sipfoundry.sipxconfig.components.ObjectSelectionModel;
import org.sipfoundry.sipxconfig.components.TapestryContext;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.device.ModelSource;
import org.sipfoundry.sipxconfig.phone.PhoneModel;

@ComponentClass(allowBody = false, allowInformalParameters = false)
public abstract class ModelSelector extends BaseComponent {

    /**
     * User ID of the user for which we are adding the phone. If set the line for this user will
     * be created on a new phone.
     */
    @Parameter(required = false)
    public abstract Integer getUserId();

    @InjectObject(value = "spring:phoneModelSource")
    public abstract ModelSource<PhoneModel> getPhoneModelSource();

    @InjectObject(value = "spring:tapestry")
    public abstract TapestryContext getTapestryContext();

    @InitialValue("createPropertySelectionModel()")
    public abstract ExtraOptionModelDecorator getPhoneSelectionModel();

    public abstract PhoneModel getPhoneModel();

    @InjectPage(NewPhone.PAGE)
    public abstract NewPhone getNewPhonePage();

    @Message("label.addNewPhone")
    public abstract String getAddNewPhoneLabel();

    protected void renderComponent(IMarkupWriter writer, IRequestCycle cycle) {
        super.renderComponent(writer, cycle);
        if (TapestryUtils.isValid(cycle, this) && TapestryUtils.isRewinding(cycle, this)) {
            addNewPhone(cycle);
        }
    }

    private void addNewPhone(IRequestCycle cycle) {
        PhoneModel model = getPhoneModel();
        if (model != null) {
            NewPhone newPhone = getNewPhonePage();
            newPhone.setPhoneModel(model);
            newPhone.setUserId(getUserId());
            newPhone.setReturnPage(getPage());
            cycle.activate(newPhone);
        }
    }

    public IPropertySelectionModel createPropertySelectionModel() {
        ObjectSelectionModel model = new ObjectSelectionModel();
        model.setCollection(getPhoneModelSource().getModels());
        model.setLabelExpression("label");

        return getTapestryContext().addExtraOption(model, getAddNewPhoneLabel());
    }
}
