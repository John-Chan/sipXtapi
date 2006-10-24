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

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.MessageDigest;

import org.apache.commons.codec.binary.Hex;
import org.apache.commons.io.IOUtils;

public class FileDigestSource {
    private static final int BUFFER_SIZE = 5000;
    private static final int MAX_DIGEST = 5;

    /**
     * Map keyed on resource path of DIGEST checksum (as a string).
     */

    public synchronized String getDigestForResource(Integer userId, String resourcePath) {
        return computeMD5(userId, resourcePath);
    }

    private String computeMD5(Integer id, String resourcePath) {

        InputStream stream = null;

        try {
            MessageDigest digest = MessageDigest.getInstance("MD5");

            stream = new FileInputStream(resourcePath);

            digest.update(id.toString().getBytes());
            digestStream(digest, stream);

            stream.close();
            stream = null;

            byte[] bytes = digest.digest();
            char[] encoded = Hex.encodeHex(bytes);

            return new String(encoded);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        } finally {
            IOUtils.closeQuietly(stream);
        }
    }

    private void digestStream(MessageDigest digest, InputStream stream) throws IOException {
        byte[] buffer = new byte[BUFFER_SIZE];

        for (int i = 0; i < MAX_DIGEST; i++) {
            int length = stream.read(buffer);

            if (length < 0) {
                return;
            }

            digest.update(buffer, 0, length);
        }
    }
}
