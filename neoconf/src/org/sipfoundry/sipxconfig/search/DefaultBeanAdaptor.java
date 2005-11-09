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
package org.sipfoundry.sipxconfig.search;

import java.io.Serializable;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.Term;
import org.hibernate.type.StringType;
import org.hibernate.type.Type;
import org.sipfoundry.sipxconfig.common.BeanWithId;

public class DefaultBeanAdaptor implements BeanAdaptor {
    private static final Log LOG = LogFactory.getLog(DefaultBeanAdaptor.class);

    // keep it sorted - used in binary search
    private static final String[] FIELDS = {
        "extension", "firstName", "lastName", "name", "userName"
    };

    /**
     * @return true if the document should be added to index
     */
    public boolean documentFromBean(Document document, Object bean, Serializable id,
            Object[] state, String[] fieldNames, Type[] types) {
        boolean addToIndex = false;
        document.add(Field.Keyword(BeanWithId.ID_PROPERTY, getKeyword(bean, id)));
        for (int i = 0; i < fieldNames.length; i++) {
            Object value = state[i];
            if (value != null && indexField(document, value, fieldNames[i], types[i])) {
                addToIndex = true;
            }
        }

        return addToIndex;
    }

    private boolean indexField(Document document, Object state, String fieldName, Type type) {
        if (Arrays.binarySearch(FIELDS, fieldName) >= 0) {
            // index all fields we know about
            document.add(Field.UnStored(fieldName, (String) state));
            document.add(Field.UnStored(Indexer.DEFAULT_FIELD, (String) state));
            return true;
        } else if (type instanceof StringType) {
            // index all strings
            document.add(Field.UnStored(Indexer.DEFAULT_FIELD, (String) state));
            return true;
        } else if (fieldName.equals("aliases")) {
            Set aliases = (Set) state;
            for (Iterator a = aliases.iterator(); a.hasNext();) {
                String alias = (String) a.next();
                document.add(Field.UnStored("alias", alias));
                document.add(Field.UnStored(Indexer.DEFAULT_FIELD, alias));
            }
            return true;
        }
        return false;
    }

    private String getKeyword(Object bean, Serializable id) {
        StringBuffer buffer = new StringBuffer();
        buffer.append(bean.getClass().getName());
        buffer.append(':');
        buffer.append(id.toString());
        return buffer.toString();
    }

    public Term getIdentityTerm(Object bean, Serializable id) {
        return new Term(BeanWithId.ID_PROPERTY, getKeyword(bean, id));
    }

    public Identity getBeanIdentity(Document document) {
        try {
            String docId = document.get("id");
            String[] ids = StringUtils.split(docId, ':');
            Class klass = Class.forName(ids[0]);
            Integer id = Integer.valueOf(ids[1]);
            return new Identity(klass, id);
        } catch (NumberFormatException e) {
            LOG.warn("invalid bean id", e);
            return null;
        } catch (ClassNotFoundException e) {
            LOG.warn("invalid bean class", e);
            return null;
        }
    }
}
