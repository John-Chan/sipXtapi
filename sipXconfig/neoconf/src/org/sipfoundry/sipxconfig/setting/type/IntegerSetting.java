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
package org.sipfoundry.sipxconfig.setting.type;

import org.apache.commons.beanutils.Converter;
import org.apache.commons.beanutils.converters.IntegerConverter;

public class IntegerSetting implements SettingType {
    /**
     * Converter should allow for null value: return null by default instead of throwing exception
     */
    private static final Converter CONVERTER = new IntegerConverter(null);

    private int m_min;

    private int m_max = Integer.MAX_VALUE;
    
    private boolean m_required;

    public int getMax() {
        return m_max;
    }

    public void setMax(int max) {
        m_max = max;
    }

    public int getMin() {
        return m_min;
    }

    public void setMin(int min) {
        m_min = min;
    }

    public String getName() {
        return "integer";
    }

    public boolean isRequired() {
        return m_required;
    }

    public void setRequired(boolean required) {
        m_required = required;
    }

    public Object convertToTypedValue(Object value) {
        return CONVERTER.convert(Integer.class, value);
    }

    public String convertToStringValue(Object value) {
        if (value == null) {
            return null;
        }
        return value.toString();
    }

    public String getLabel(Object value) {
        return convertToStringValue(value);
    }    
}
