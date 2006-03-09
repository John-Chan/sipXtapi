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
package org.sipfoundry.sipxconfig.components;

import junit.framework.TestCase;

import org.apache.hivemind.Messages;
import org.apache.tapestry.contrib.table.model.ITableColumn;
import org.apache.tapestry.contrib.table.model.simple.SimpleTableColumn;
import org.apache.tapestry.valid.RenderString;
import org.easymock.MockControl;

public class LocalizedTableRendererSourceTest extends TestCase {

    private final class TestColumn extends SimpleTableColumn {
        private TestColumn(String name) {
            super(name);
        }

        public Object getColumnValue(Object objRow) {
            if ("value1".equals(objRow)) {
                return null;
            }
            if ("value2".equals(objRow)) {
                return "bongo";
            }
            fail("should be only called with value1, or value2 as parameter");
            return null;
        
        }
    }

    public void testGetRenderer() {
        MockControl messagesCtrl = MockControl.createControl(Messages.class);
        Messages messages = (Messages) messagesCtrl.getMock();
        
        // PORT
        //messages.getMessage("prefix.bongo", "bongo");
        messages.getMessage("prefix.bongo");
        
        messagesCtrl.setReturnValue("kuku");
        messagesCtrl.replay();

        // class mock does not work here...
        ITableColumn column = new TestColumn(null);

        LocalizedTableRendererSource source = new LocalizedTableRendererSource(messages, "prefix");

        RenderString renderer1 = (RenderString) source.getRenderer(null, null, column, "value1");
        assertEquals("&nbsp;", renderer1.getString());

        RenderString renderer2 = (RenderString) source.getRenderer(null, null, column, "value2");
        assertEquals("kuku", renderer2.getString());

        messagesCtrl.verify();
    }
    
    public void testGetRendererNoPrefix() {
        MockControl messagesCtrl = MockControl.createControl(Messages.class);
        Messages messages = (Messages) messagesCtrl.getMock();
        messages.getMessage("bongo");
        messagesCtrl.setReturnValue("kuku");
        messagesCtrl.replay();

        // class mock does not work here...
        ITableColumn column = new TestColumn(null);

        LocalizedTableRendererSource source = new LocalizedTableRendererSource(messages);

        RenderString renderer2 = (RenderString) source.getRenderer(null, null, column, "value2");
        assertEquals("kuku", renderer2.getString());

        messagesCtrl.verify();
    }
    

}
