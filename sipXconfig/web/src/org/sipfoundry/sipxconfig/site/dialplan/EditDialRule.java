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
package org.sipfoundry.sipxconfig.site.dialplan;

import org.apache.tapestry.IPage;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.callback.ICallback;
import org.apache.tapestry.callback.PageCallback;
import org.apache.tapestry.event.PageBeginRenderListener;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.admin.dialplan.DialPlanContext;
import org.sipfoundry.sipxconfig.admin.dialplan.DialingRule;
import org.sipfoundry.sipxconfig.admin.dialplan.DialingRuleType;
import org.sipfoundry.sipxconfig.permission.PermissionManager;

/**
 * EditDialRule
 */
public abstract class EditDialRule extends BasePage implements PageBeginRenderListener {

    public static final String CUSTOM = "EditCustomDialRule";
    public static final String INTERNAL = "EditInternalDialRule";
    public static final String ATTENDANT = "EditAttendantDialRule";
    public static final String LOCAL = "EditLocalDialRule";
    public static final String LONG_DISTANCE = "EditLongDistanceDialRule";
    public static final String EMERGENCY = "EditEmergencyDialRule";
    public static final String INTERNATIONAL = "EditInternationalDialRule";

    public abstract DialPlanContext getDialPlanContext();

    public abstract Integer getRuleId();

    public abstract void setRuleId(Integer ruleId);

    public abstract DialingRule getRule();

    public abstract void setRule(DialingRule rule);

    public abstract ICallback getCallback();

    public abstract void setCallback(ICallback callback);
    
    public abstract DialingRuleType getRuleType();

    public abstract void setRuleType(DialingRuleType dialingType);
    
    public abstract PermissionManager getPermissionManager();

    public void pageBeginRender(PageEvent event_) {
        DialingRule rule = getRule();
        if (null != rule) {
            // FIXME: in custom rules: rule is persitent but rule type not...
            setRuleType(rule.getType());
            rule.setPermissionManager(getPermissionManager());
            return;
        }
        Integer id = getRuleId();
        if (null != id) {
            DialPlanContext manager = getDialPlanContext();
            rule = manager.getRule(id);
            setRuleType(rule.getType());
        } else {
            rule = getRuleType().create();
            rule.setPermissionManager(getPermissionManager());
        }
        setRule(rule);

        // Ignore the callback passed to us for now because we're navigating
        // to unexpected places. Always go to the EditFlexibleDialPlan plan.
        setCallback(new PageCallback(EditFlexibleDialPlan.PAGE));
    }

    /**
     * Go to emergency routing page. Set callback to return to this page. Used only in
     * EditEmergencyRouting rule
     */
    public IPage emergencyRouting(IRequestCycle cycle) {
        EditEmergencyRouting page = (EditEmergencyRouting) cycle
                .getPage(EditEmergencyRouting.PAGE);
        PageCallback callback = new PageCallback(getPage());
        page.setCallback(callback);
        return page;
    }
}
