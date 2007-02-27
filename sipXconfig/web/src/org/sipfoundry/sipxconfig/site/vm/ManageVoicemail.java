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
package org.sipfoundry.sipxconfig.site.vm;

import java.io.File;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.tapestry.IAsset;
import org.apache.tapestry.IExternalPage;
import org.apache.tapestry.IPage;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.PageRedirectException;
import org.apache.tapestry.annotations.Asset;
import org.apache.tapestry.annotations.InjectObject;
import org.apache.tapestry.annotations.Persist;
import org.apache.tapestry.bean.EvenOdd;
import org.apache.tapestry.components.IPrimaryKeyConverter;
import org.apache.tapestry.contrib.table.model.ITableColumn;
import org.apache.tapestry.engine.IEngineService;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.form.IPropertySelectionModel;
import org.apache.tapestry.services.ExpressionEvaluator;
import org.apache.tapestry.valid.ValidatorException;
import org.sipfoundry.sipxconfig.common.UserException;
import org.sipfoundry.sipxconfig.components.RowInfo;
import org.sipfoundry.sipxconfig.components.SelectMap;
import org.sipfoundry.sipxconfig.components.TapestryContext;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.components.selection.AdaptedSelectionModel;
import org.sipfoundry.sipxconfig.components.selection.OptGroup;
import org.sipfoundry.sipxconfig.components.selection.OptionAdapter;
import org.sipfoundry.sipxconfig.site.user_portal.UserBasePage;
import org.sipfoundry.sipxconfig.vm.MailboxManager;
import org.sipfoundry.sipxconfig.vm.Voicemail;
import org.sipfoundry.sipxconfig.vm.VoicemailSource;

public abstract class ManageVoicemail extends UserBasePage implements IExternalPage {
    
    public static final String PAGE = "vm/ManageVoicemail";
        
    private static final Log LOG = LogFactory.getLog(ManageVoicemail.class);

    @Asset("/images/voicemail-play.png")
    public abstract IAsset getPlayVoicemailAsset();

    @InjectObject(value = "spring:mailboxManager")
    public abstract MailboxManager getMailboxManager();
    
    @InjectObject(value = "spring:tapestry")
    public abstract TapestryContext getTapestry();

    public abstract VoicemailSource getVoicemailSource();
    public abstract void setVoicemailSource(VoicemailSource source);

    public abstract SelectMap getSelections();
    public abstract void setSelections(SelectMap selections);

    public abstract Voicemail getVoicemail();

    public abstract List<Voicemail> getVoicemails();

    public abstract void setVoicemails(List<Voicemail> vm);

    @InjectObject(value = "service:tapestry.ognl.ExpressionEvaluator")
    public abstract ExpressionEvaluator getExpressionEvaluator();

    public abstract void setConverter(IPrimaryKeyConverter converter);
    
    public abstract VoicemailRowInfo getRowInfo();
    public abstract void setRowInfo(VoicemailRowInfo rowInfo);
    
    public abstract List<String> getFolderIds();
    public abstract void setFolderIds(List<String> folderIds);
    
    @Persist(value = "session")
    public abstract String getFolderId();
    public abstract void setFolderId(String folderId);
    
    @InjectObject(value = "engine-service:" + PlayVoicemailService.SERVICE_NAME)
    public abstract IEngineService getPlayVoicemailService();
    
    public abstract MailboxOperation getMailboxOperation();
    public abstract void setMailboxOperation(MailboxOperation operation);
    
    public Object getRowClass() {
        return new HeardEvenOdd(this);
    }
    
    public class HeardEvenOdd extends EvenOdd {
        private ManageVoicemail m_page;
        
        public HeardEvenOdd(ManageVoicemail page) {
            m_page = page;
        }

        @Override
        public String getNext() {
            String style = super.getNext();
            if (!m_page.getVoicemail().isHeard()) {
                style = style + "-unheard";
            }
            return style;
        }        
    }

    public void activateExternalPage(Object[] parameters, IRequestCycle cycle) {
        String sparam = parameters[0].toString();
        MailboxOperation operation = MailboxOperation.createMailboxOperationFromServletPath(sparam);        
        setFolderId(operation.getFolderId());
        setMailboxOperation(operation);
    }

    public IPropertySelectionModel getActionModel() {
        Collection actions = new ArrayList();
        actions.add(new OptGroup(getMessages().getMessage("label.moveTo")));
        for (String folderId : getFolderIds()) {
            if (!folderId.equals(getFolderId())) {
                OptionAdapter action = new MoveVoicemailAction(getVoicemailSource(), 
                        getFolderLabel(folderId), folderId); 
                actions.add(action);
            }
        }
        
        AdaptedSelectionModel model = new AdaptedSelectionModel();
        model.setCollection(actions);
        return model;
    }
    
    public void delete() {
        Collection<Serializable> allSelected =  getSelections().getAllSelected();
        for (Serializable id : allSelected) {
            Voicemail vm = getVoicemailSource().getVoicemail(id);
            vm.delete();
        }        
    }
    
    public String getFolderLabel() {
        return getFolderLabel(getFolderId());
    }
    
    String getFolderLabel(String folderId) {
        return getMessages().getMessage("tab." + folderId);
    }
    
    public IPage edit(String voicemailId) {
        EditVoicemail page = (EditVoicemail) getRequestCycle().getPage(EditVoicemail.PAGE);
        page.setReturnPage(PAGE);
        page.setVoicemailId(voicemailId);
        return page;        
    }

    public ITableColumn getTimestampColumn() {
        return TapestryUtils.createDateColumn("descriptor.timestamp", getMessages(),
                getExpressionEvaluator(), getLocale());
    }
    
    public PlayVoicemailService.Info getPlayVoicemailInfo() {
        Voicemail voicemail = getVoicemail();
        PlayVoicemailService.Info info = new PlayVoicemailService.Info(voicemail.getFolderId(), 
                voicemail.getMessageId());
        return info;
    }

    public void pageBeginRender(PageEvent event) {
        super.pageBeginRender(event);
        
        // this needs to be first as it may alter data gathered from subsequent steps in this method
        MailboxOperation operation = getMailboxOperation();        
        if (operation != null) {
            String expectedUserId = getUser().getUserName();
            if (!expectedUserId.equals(operation.getUserId())) {
                String msg = String.format("Unauthorized access attempted to mailbox for user %s from user %s", 
                        operation.getUserId(), expectedUserId); 
                LOG.warn(msg);
                throw new PageRedirectException(msg);
            }            
            setMailboxOperation(null);
            operation.operate(this);
        }        
        
        SelectMap selections = getSelections();
        if (selections == null) {
            setSelections(new SelectMap());
        }
        
        String userId = getUser().getUserName();
        
        List<String> folderIds = getFolderIds(); 
        if (getFolderIds() == null) {
            folderIds = getMailboxManager().getFolderIds(userId);
            setFolderIds(folderIds);
        }
        
        String folderId = getFolderId();
        if (folderId == null || !folderIds.contains(folderId)) {
            setFolderId(folderIds.get(0));
        }

        List<Voicemail> vm;
        try {
            vm = getMailboxManager().getVoicemail(userId, getFolderId());
        } catch (UserException e) {
            getValidator().record(new ValidatorException(e.getMessage()));
            vm = Collections.emptyList();
        }
        setVoicemails(vm);
        
        VoicemailSource source = getVoicemailSource();
        if (source == null) {            
            source = new VoicemailSource(new File(getMailboxManager().getMailstoreDirectory()));
            setVoicemailSource(source);
            setRowInfo(new VoicemailRowInfo(source));
            setConverter(new VoicemailSqueezer(source));
        }        
    }
    
    public static class VoicemailRowInfo implements RowInfo<Voicemail> {
        private VoicemailSource m_source;
        
        VoicemailRowInfo(VoicemailSource source) {
            m_source = source;
        }

        public Object getSelectId(Voicemail row) {
            return m_source.getVoicemailId(row);
        }

        public boolean isSelectable(Voicemail row) {
            return true;
        }        
    }
}