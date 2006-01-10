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
package org.sipfoundry.sipxconfig.admin.commserver.imdb;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.Charset;

import org.apache.commons.codec.binary.Base64;
import org.apache.commons.io.IOUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.dom4j.Document;
import org.dom4j.DocumentFactory;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;
import org.sipfoundry.sipxconfig.admin.commserver.Location;
import org.sipfoundry.sipxconfig.admin.dialplan.config.XmlFile;

public class ReplicationManagerImpl implements ReplicationManager {
    private static final Log LOG = LogFactory.getLog(ReplicationManagerImpl.class);
    private static final Charset CHARSET_UTF8 = Charset.forName("UTF8");

    /**
     * sends payload data to all URLs
     * 
     * It only returns one result, if there is a failure checking the log is the only way to
     * detect it. We could throw exceptions from here but it would mean that a single IO failure
     * dooms entire replication process.
     */
    public boolean replicateData(Location[] locations, DataSetGenerator generator, DataSet type) {

        boolean success = true;
        for (int i = 0; i < locations.length; i++) {
            try {
                generator.setSipDomain(locations[i].getSipDomain());
                Document payload = generator.generate();
                byte[] payloadBytes = xmlToByteArray(payload, false);
                Document xml = generateXMLDataToPost(payloadBytes, type.getName(), "database");
                byte[] data = xmlToByteArray(xml, false);
                postData(locations[i].getReplicationUrl(), data);
            } catch (IOException e) {
                success = false;
                LOG.error("Data replication failed: " + type.getName(), e);
            }
        }
        return success;
    }

    private byte[] xmlToByteArray(Document doc, boolean niceFormat) {
        OutputFormat format = new OutputFormat();
        if (niceFormat) {
            format.setNewlines(true);
            format.setIndent(true);
        }
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        Writer writer = new BufferedWriter(new OutputStreamWriter(stream, CHARSET_UTF8));
        try {
            XMLWriter xmlWriter = new XMLWriter(writer, format);
            xmlWriter.write(doc);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            IOUtils.closeQuietly(writer);
        }
        return stream.toByteArray();
    }

    protected HttpURLConnection getConnection(String url) throws IOException {
        URL replicateURL = new URL(url);
        return (HttpURLConnection) replicateURL.openConnection();
    }

    /**
     * posts the xmlData to the URL given.
     */
    boolean postData(String url, byte[] xmlData) throws IOException {
        HttpURLConnection urlConn = getConnection(url);
        urlConn.setDoOutput(true);
        urlConn.setRequestMethod("POST");
        // it's optional - need to check if our server can handle it
        urlConn.setRequestProperty("Content-length", String.valueOf(xmlData.length));
        urlConn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
        urlConn.getOutputStream().write(xmlData);
        urlConn.connect();

        String strResponseMessage = urlConn.getResponseMessage();
        LOG.debug("Response message: " + strResponseMessage);
        LOG.debug("Error response: " + urlConn.getHeaderField("ErrorInReplication"));

        BufferedReader reader = new BufferedReader(
                new InputStreamReader(urlConn.getInputStream()));
        String strResponseBody = reader.readLine(); 
        return strResponseBody != null && strResponseBody.startsWith("replication was successful");
    }

    /**
     * generates xml data to post to a URL
     * 
     * @param dataType database or file - depending on type of data to replicate
     */
    Document generateXMLDataToPost(byte[] payload, String targetDataName, String dataType) {
        byte[] encodedPayload = Base64.encodeBase64(payload);
        // Base64 encoded content is always limited to US-ASCII charset
        String strPayload;
        try {
            strPayload = new String(encodedPayload, "US-ASCII");
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }

        Document document = DocumentFactory.getInstance().createDocument();
        Element data = document.addElement("replicationdata").addElement("data");
        data.addAttribute("type", dataType);
        data.addAttribute("action", "replace");

        data.addAttribute("target_data_name", targetDataName);

        // these 2 values are hardcoded for now - they do not seem to be used by replication.cgi
        data.addAttribute("target_component_type", "comm-server");
        data.addAttribute("target_component_id", "CommServer1");

        data.addElement("payload").setText(strPayload);

        return document;
    }

    public boolean replicateFile(Location[] locations, XmlFile file) {
        boolean success = true;
        for (int i = 0; i < locations.length; i++) {
            try {
                Document payload = file.getDocument();
                byte[] payloadBytes = xmlToByteArray(payload, true);
                Document xml = generateXMLDataToPost(payloadBytes, file.getType().getName(),
                        "file");
                byte[] data = xmlToByteArray(xml, false);
                postData(locations[i].getReplicationUrl(), data);
            } catch (IOException e) {
                success = false;
                LOG.error("File replication failed: " + file.getType().getName(), e);
            }
        }
        return success;
    }
}
