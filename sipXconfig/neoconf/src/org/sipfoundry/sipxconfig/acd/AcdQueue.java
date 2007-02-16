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
package org.sipfoundry.sipxconfig.acd;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.ObjectUtils;
import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.common.DataCollectionUtil;
import org.sipfoundry.sipxconfig.common.SipUri;
import org.sipfoundry.sipxconfig.setting.AbstractSettingVisitor;
import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingEntry;
import org.sipfoundry.sipxconfig.setting.type.FileSetting;
import org.sipfoundry.sipxconfig.setting.type.SettingType;

public class AcdQueue extends AcdComponent {
    public static final String BEAN_NAME = "acdQueue";
    public static final String OBJECT_CLASS = "acd-queue";

    static final String URI = "acd-queue/uri";
    static final String QUEUE_NAME = "acd-queue/name";
    static final String WELCOME_AUDIO = "acd-queue/welcome-audio";
    static final String QUEUE_AUDIO = "acd-queue/queue-audio";
    static final String BACKGROUND_AUDIO = "acd-queue/background-audio";
    static final String CALL_TERMINATION_AUDIO = "acd-queue/call-termination-audio";
    static final String AGENT_LIST = "acd-queue/acd-agent-list";
    static final String OVERFLOW_QUEUE = "acd-queue/overflow-queue";

    static final String[] AUDIO_SETTINGS = {
        WELCOME_AUDIO, QUEUE_AUDIO, BACKGROUND_AUDIO, CALL_TERMINATION_AUDIO
    };

    private List m_agents = new ArrayList();

    private Set m_lines = new HashSet();

    private AcdServer m_acdServer;

    private AcdQueue m_overflowQueue;
    private String m_audioDirectory;

    public AcdQueue() {
        super("sipxacd-queue.xml", OBJECT_CLASS);
    }

    public List getAgents() {
        return m_agents;
    }

    public void setAgents(List agents) {
        m_agents = agents;
    }

    public Set getLines() {
        return m_lines;
    }

    public void setLines(Set lines) {
        m_lines = lines;
    }

    public AcdServer getAcdServer() {
        return m_acdServer;
    }

    public void setAcdServer(AcdServer acdServer) {
        m_acdServer = acdServer;
    }

    public AcdQueue getOverflowQueue() {
        return m_overflowQueue;
    }

    public void setOverflowQueue(AcdQueue overflowQueue) {
        m_overflowQueue = overflowQueue;
    }

    public String getQueueAudio() {
        return getSettingValue(QUEUE_AUDIO);
    }

    public String getWelcomeAudio() {
        return getSettingValue(WELCOME_AUDIO);
    }

    public String getBackgroundAudio() {
        return getSettingValue(BACKGROUND_AUDIO);
    }

    public String getCallTerminationAudio() {
        return getSettingValue(CALL_TERMINATION_AUDIO);
    }

    /**
     * Return list of all audio files referenced by this queue.
     */
    public List getAudioFiles() {
        List audioFiles = new ArrayList();
        for (int i = 0; i < AUDIO_SETTINGS.length; i++) {
            String audio = getSettingValue(AUDIO_SETTINGS[i]);
            if (StringUtils.isNotBlank(audio)) {
                audioFiles.add(audio);
            }
        }
        return audioFiles;
    }

    public String getAudioDirectory() {
        return m_audioDirectory;
    }

    public void setAudioDirectory(String audioDirectory) {
        m_audioDirectory = audioDirectory;
    }

    public void insertAgent(AcdAgent acdAgent) {
        m_agents.remove(acdAgent);
        m_agents.add(acdAgent);
        acdAgent.addToQueue(this);
    }

    public void removeAgent(AcdAgent acdAgent) {
        m_agents.remove(acdAgent);
        acdAgent.removeFromQueue(this);
    }

    public void removeLine(AcdLine line) {
        getLines().remove(line);
    }

    public void addLine(AcdLine line) {
        getLines().add(line);
    }

    @Override
    public String calculateUri() {
        String domainName = getAcdServer().getHost();
        return SipUri.format(getName(), domainName, false);
    }

    private String[] getAgentUris() {
        final int agentsLen = getAgents().size();
        String[] agentNames = new String[agentsLen];
        for (int i = 0; i < agentsLen; i++) {
            AcdAgent agent = (AcdAgent) getAgents().get(i);
            agentNames[i] = agent.calculateUri();
        }
        return agentNames;
    }

    private static class AudioDirectorySetter extends AbstractSettingVisitor {
        private String m_audioDirectory;

        public AudioDirectorySetter(String directory) {
            m_audioDirectory = directory;
        }

        public void visitSetting(Setting setting) {
            SettingType type = setting.getType();
            if (type instanceof FileSetting) {
                FileSetting fileType = (FileSetting) type;
                fileType.setDirectory(m_audioDirectory);
            }
        }
    }

    public static class AcdQueueDefaults {
        private AcdQueue m_queue;

        AcdQueueDefaults(AcdQueue queue) {
            m_queue = queue;
        }

        @SettingEntry(path = AGENT_LIST)
        public String getAgentList() {
            String list = StringUtils.join(m_queue.getAgentUris(), ',');
            return list;
        }

        @SettingEntry(path = QUEUE_NAME)
        public String getQueueName() {
            String acdName = (String) ObjectUtils.defaultIfNull(m_queue.getDescription(), m_queue
                    .getName());
            return acdName;
        }

        @SettingEntry(path = URI)
        public String getUri() {
            return m_queue.calculateUri();
        }

        @SettingEntry(path = OVERFLOW_QUEUE)
        public String getOverflowQueue() {
            String overflowQueueValue;
            AcdQueue overflowQueue = m_queue.getOverflowQueue();
            if (overflowQueue != null) {
                overflowQueueValue = overflowQueue.calculateUri();
            } else {
                overflowQueueValue = StringUtils.EMPTY;
            }

            return overflowQueueValue;
        }
    }

    public void cleanLines() {
        Set lines = getLines();
        for (Iterator i = lines.iterator(); i.hasNext();) {
            AcdLine line = (AcdLine) i.next();
            line.setAcdQueue(null);
        }
        lines.clear();
    }

    public void cleanAgents() {
        Collection agents = getAgents();
        for (Iterator i = agents.iterator(); i.hasNext();) {
            AcdAgent agent = (AcdAgent) i.next();
            agent.removeFromQueue(this);
        }
        agents.clear();
    }

    public void moveAgents(Collection ids, int step) {
        DataCollectionUtil.moveByPrimaryKey(getAgents(), ids.toArray(), step, false);
    }

    public Serializable getAcdServerId() {
        return getAcdServer().getId();
    }

    @Override
    public void initialize() {
        addDefaultBeanSettingHandler(new AcdQueueDefaults(this));

        AudioDirectorySetter audioDirectorySetter = new AudioDirectorySetter(m_audioDirectory);
        getSettings().acceptVisitor(audioDirectorySetter);
    }
}
