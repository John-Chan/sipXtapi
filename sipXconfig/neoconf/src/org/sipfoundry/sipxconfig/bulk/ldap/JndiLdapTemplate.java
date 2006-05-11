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
package org.sipfoundry.sipxconfig.bulk.ldap;

import java.util.Properties;

import javax.naming.Context;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;
import javax.naming.directory.DirContext;
import javax.naming.directory.InitialDirContext;
import javax.naming.directory.SearchControls;
import javax.naming.directory.SearchResult;

import org.springframework.jndi.JndiCallback;
import org.springframework.jndi.JndiTemplate;

/**
 * Special version of JndiTemlated better suited for LDAP context.
 * 
 * Provides setters for commonly used properties. Uses InitialDirContext and not generic
 * InitialContext class.
 */
public class JndiLdapTemplate extends JndiTemplate {
    protected Context createInitialContext() throws NamingException {
        return new InitialDirContext(getEnvironment());
    }

    public NamingEnumeration<SearchResult> search(final String name, final String filter,
            final SearchControls cons) throws NamingException {

        return (NamingEnumeration<SearchResult>) execute(new JndiCallback() {
            public Object doInContext(Context ctx) throws NamingException {
                DirContext dirContext = (DirContext) ctx;

                return dirContext.search(name, filter, cons);
            }
        });

    }

    public Properties getEnvironment() {
        Properties environment = super.getEnvironment();
        if (environment == null) {
            environment = new Properties();
            setEnvironment(environment);
        }
        return environment;
    }

    public void setSecurityAuthentication(String value) {
        getEnvironment().setProperty(Context.SECURITY_AUTHENTICATION, value);
    }

    public void setSecurityPrincipal(String value) {
        getEnvironment().setProperty(Context.SECURITY_PRINCIPAL, value);
    }

    public void setSecurityCredentials(String value) {
        getEnvironment().setProperty(Context.SECURITY_CREDENTIALS, value);
    }

    public void setIntialContextFactory(String value) {
        getEnvironment().put(Context.INITIAL_CONTEXT_FACTORY, value);
    }

    public void setProviderUrl(String value) {
        getEnvironment().put(Context.PROVIDER_URL, value);
    }

}
