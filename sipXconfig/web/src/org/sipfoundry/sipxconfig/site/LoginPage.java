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
package org.sipfoundry.sipxconfig.site;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.tapestry.PageRedirectException;
import org.apache.tapestry.callback.ICallback;
import org.apache.tapestry.event.PageBeginRenderListener;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.html.BasePage;
import org.apache.tapestry.valid.IValidationDelegate;
import org.apache.tapestry.valid.ValidationConstraint;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.login.LoginContext;
import org.sipfoundry.sipxconfig.site.user.FirstUser;

public abstract class LoginPage extends BasePage implements PageBeginRenderListener {

    public static final String PAGE = "LoginPage";

    private static final Log LOG = LogFactory.getLog(LoginPage.class);

    public abstract CoreContext getCoreContext();

    public abstract LoginContext getLoginContext();

    public abstract String getPassword();

    public abstract void setPassword(String password);

    public abstract String getUserName();

    public abstract void setUserName(String userName);

    public abstract ICallback getCallback();

    public abstract void setCallback(ICallback callback);

    public abstract UserSession getUserSession();

    public void pageBeginRender(PageEvent event) {
        
        // If there are no users in the DB, then redirect to the FirstUser page to make one.
        // For most pages, Border takes care of this check, but LoginPage doesn't have a Border.
        int userCount = getCoreContext().getUsersCount();
        if (userCount == 0) {
            throw new PageRedirectException(FirstUser.PAGE);
        }

        // If there is only one user in the DB, then this is probably a brand-new system where
        // we just created the superadmin user. Fill in the username automatically to be
        // helpful to the IT guy, who may be learning about sipXconfig for the first time.
        if (userCount == 1) {
            List users = getCoreContext().loadUsers();
            User user = (User) users.get(0);
            setUserName(user.getUserName());
        }
    }

    public String login() {
        // always clean password property - use local variable in this function
        String password = getPassword();
        setPassword(null);

        if (!TapestryUtils.isValid(this)) {
            return null;
        }

        LoginContext context = getLoginContext();
        User user = context.checkCredentials(getUserName(), password);
        if (user == null) {
            IValidationDelegate delegate = (IValidationDelegate) getBeans().getBean("validator");
            delegate.record(getMessages().getMessage("message.loginError"),
                    ValidationConstraint.CONSISTENCY);
            return null;
        }

        UserSession userSession = getUserSession();
        userSession.login(user.getId(), context.isAdmin(user), user.isSupervisor());

        // Ignore any callback and go to the home page. If the user was redirected to the login
        // page because the session timed out, then after logging in we will have lost all session
        // data. Trying to execute a callback under these circumstances is hazardous -- see
        // XCF-590.
        return Home.PAGE;
    }
    
    public String getVoicemailHref() {
        String root = "/";
        String home = getRequestCycle().getAbsoluteURL(root);
        try {
            URL url = new URL(home);
            URL voicemail = new URL(url.getProtocol(), url.getHost(), 8091, root);
            return voicemail.toString();
        } catch (MalformedURLException e) {
            LOG.error(e);
            return home;
        }
    }
}
