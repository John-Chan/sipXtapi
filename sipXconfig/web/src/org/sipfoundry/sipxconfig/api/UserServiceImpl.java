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
package org.sipfoundry.sipxconfig.api;

import java.rmi.RemoteException;
import java.util.Collections;
import java.util.List;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.setting.Group;
import org.sipfoundry.sipxconfig.setting.SettingDao;

public class UserServiceImpl implements UserService {
    
    /** TODO: Remove this */
    private static final int PAGE_SIZE = 1000;
    
    private static final String SORT_ORDER = "userName";
    
    private static final Log LOG = LogFactory.getLog(UserServiceImpl.class);
    
    private CoreContext m_coreContext;
    
    private SettingDao m_settingDao;
    
    private UserBuilder m_userBuilder;
    
    public void setCoreContext(CoreContext coreContext) {
        m_coreContext = coreContext;
    }
    
    public void setSettingDao(SettingDao settingDao) {
        m_settingDao = settingDao;
    }

    public void setUserBuilder(UserBuilder userTranslater) {
        m_userBuilder = userTranslater;
    }
    
    public void addUser(AddUser addUser) throws RemoteException {
        org.sipfoundry.sipxconfig.common.User myUser = new org.sipfoundry.sipxconfig.common.User();
        User apiUser = addUser.getUser();        
        ApiBeanUtil.toMyObject(m_userBuilder, myUser, apiUser);
        String[] groups = addUser.getGroup();
        String resourceId = org.sipfoundry.sipxconfig.common.User.GROUP_RESOURCE_ID;
        for (int i = 0; groups != null && i < groups.length; i++) {
            Group g = m_settingDao.getGroupCreateIfNotFound(resourceId, groups[i]);
            myUser.addGroup(g);
        }
        myUser.setPin(addUser.getPin(), m_coreContext.getAuthorizationRealm());
        m_coreContext.saveUser(myUser);
    }
    
    public FindUserResponse findUser(FindUser findUser) throws RemoteException {
        FindUserResponse response = new FindUserResponse();        
        org.sipfoundry.sipxconfig.common.User[] users = search(findUser.getSearch());
        User[] arrayOfUsers = (User[]) ApiBeanUtil.toApiArray(m_userBuilder, users, User.class);
        response.setUsers(arrayOfUsers);
        
        return response;
    }

    org.sipfoundry.sipxconfig.common.User[] search(UserSearch search) {
        List users = Collections.EMPTY_LIST;
        if (search == null) {
            users = m_coreContext.loadUsers();
        } else if (search.getByUserName() != null) {
            org.sipfoundry.sipxconfig.common.User user = m_coreContext.loadUserByUserName(search.getByUserName());
            if (user != null) {
                users = Collections.singletonList(user);
            }
        } else if (search.getByFuzzyUserNameOrAlias() != null) {
            users = m_coreContext.loadUsersByPage(search.getByFuzzyUserNameOrAlias(),
                    null, 0, PAGE_SIZE, SORT_ORDER, true);                
            warnIfOverflow(users, PAGE_SIZE);
        } else if (search.getByGroup() != null) {
            String resourceId = org.sipfoundry.sipxconfig.common.User.GROUP_RESOURCE_ID;
            Group g = m_settingDao.getGroupByName(resourceId, search.getByGroup());
            users = m_coreContext.loadUsersByPage(null, g.getId(), 0, PAGE_SIZE, SORT_ORDER, true);
            warnIfOverflow(users, PAGE_SIZE);
        }
        
        return (org.sipfoundry.sipxconfig.common.User[])
            users.toArray(new org.sipfoundry.sipxconfig.common.User[users.size()]);
    }
    
    void warnIfOverflow(List list, int size) {
        if (list.size() >= size) {
            LOG.warn("Search results exceeded maximum size " + PAGE_SIZE);
        }        
    }

    public void editUser(EditUser editUser) throws RemoteException {
        org.sipfoundry.sipxconfig.common.User[] myUsers = search(editUser.getSearch());
        Set properties  = ApiBeanUtil.getSpecfiedProperties(editUser.getProperties());
        for (int i = 0; i < myUsers.length; i++) {
            User apiUser = new User();
            ApiBeanUtil.setProperties(apiUser, editUser.getProperties());
            m_userBuilder.toMyObject(myUsers[i], apiUser, properties);
            // TODO: lines and groups
            m_coreContext.saveUser(myUsers[i]);
        }
    }

    public void manageUser(ManageUser manageUser) throws RemoteException {
        org.sipfoundry.sipxconfig.common.User[] myUsers = search(manageUser.getSearch());
        for (int i = 0; i < myUsers.length; i++) {
            if (Boolean.TRUE.equals(manageUser.getDoDelete())) {
                m_coreContext.deleteUser(myUsers[i]);
            }
        }
    }
}
