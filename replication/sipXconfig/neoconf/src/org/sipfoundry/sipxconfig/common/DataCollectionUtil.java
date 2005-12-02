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
package org.sipfoundry.sipxconfig.common;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.commons.collections.CollectionUtils;

public final class DataCollectionUtil {

    private DataCollectionUtil() {
        // prevent creation
    }

    /**
     * Removes items from collections by their primary key and updates the positions on the items
     * 
     * @param c items must implement DataCollectionItem
     */
    public static Collection removeByPrimaryKey(Collection c, Object[] primaryKeys) {

        Collection removed = findByPrimaryKey(c, primaryKeys);
        Iterator remove = removed.iterator();
        while (remove.hasNext()) {
            c.remove(remove.next());
        }

        updatePositions(c);

        return removed;
    }

    /**
     * Given a list of objects that know their primary keys, extract a collection of just primary
     * keys
     * 
     * @param c collection of PrimaryKeySource objects
     * @return collecion of primary keys
     */
    public static Collection extractPrimaryKeys(Collection c) {
        ArrayList list = new ArrayList(c.size());
        for (Iterator i = c.iterator(); i.hasNext();) {
            list.add(((PrimaryKeySource) i.next()).getPrimaryKey());
        }
        return list;
    }

    /**
     * Fixes position fields after a list manipulation.
     */
    public static void updatePositions(Collection c) {
        // update positions
        Iterator update = c.iterator();
        for (int i = 0; update.hasNext(); i++) {
            DataCollectionItem item = (DataCollectionItem) update.next();
            item.setPosition(i);
        }
    }

    /**
     * Return list of items by their primary key. This may not be efficient for large collections.
     */
    public static Collection findByPrimaryKey(Collection c, Object[] primaryKeys) {
        List list = new ArrayList();

        // mark items to be deleted
        Iterator remove = c.iterator();
        while (remove.hasNext()) {
            PrimaryKeySource item = (PrimaryKeySource) remove.next();
            for (int j = 0; j < primaryKeys.length; j++) {
                if (item.getPrimaryKey().equals(primaryKeys[j])) {
                    list.add(item);
                }
            }
        }

        return list;
    }

    /**
     * Version of moveByPrimary keys that only works on collections of objects implementing
     * DataCollectionItem interface
     */
    public static void moveByPrimaryKey(List c, Object[] primaryKeys, int step) {
        moveByPrimaryKey(c, primaryKeys, step, true);
    }

    /**
     * Moves items from collections by their primary key and updates the positions on the items.
     * Items that are attempted to move out of list bounds are quietly moved to beginning or end
     * of list.
     * 
     * @param step how many slots to move items, positive or negative
     * @param c items must implement DataCollectionItem
     */
    public static void moveByPrimaryKey(List c, Object[] primaryKeys, int step,
            boolean updatePositions) {
        Set toBeMovedSet = new HashSet(primaryKeys.length);
        CollectionUtils.addAll(toBeMovedSet, primaryKeys);

        if (step < 0) {
            // move up
            int minPosition = 0;
            for (int i = 0; i < c.size() && !toBeMovedSet.isEmpty(); i++) {
                PrimaryKeySource item = (PrimaryKeySource) c.get(i);
                if (toBeMovedSet.remove(item.getPrimaryKey())) {
                    int newPosition = Math.max(minPosition, i + step);
                    c.remove(i);
                    c.add(newPosition, item);
                    minPosition = newPosition + 1;
                }
            }
        } else if (step > 0) {
            // move down
            int maxPosition = c.size() - 1;
            for (int i = c.size() - 1; i >= 0 && !toBeMovedSet.isEmpty(); i--) {
                PrimaryKeySource item = (PrimaryKeySource) c.get(i);
                if (toBeMovedSet.remove(item.getPrimaryKey())) {
                    int newPosition = Math.min(i + step, maxPosition);
                    c.remove(i);
                    c.add(newPosition, item);
                    maxPosition = newPosition - 1;
                }
            }
        }

        if (updatePositions) {
            updatePositions(c);
        }
    }

    /**
     * Deep copy for bean collection. Duplicate and not clone is called which makes it hibernate
     * friendly. (all new objects have UNSAVED id set).
     * 
     * We do not want to deal with cloning the collection itself. The caller has to provide a
     * newly created (or at least empty) destination collection.
     * 
     * @param from source collection
     * @param to destination (empty collection)
     */
    public static void duplicate(Collection from, Collection to) {
        if (to.size() > 0) {
            throw new IllegalArgumentException("destination collection has to be empty");
        }
        for (Iterator i = from.iterator(); i.hasNext();) {
            BeanWithId bean = (BeanWithId) i.next();
            to.add(bean.duplicate());
        }
    }
}
