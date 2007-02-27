package org.sipfoundry.sipxconfig.phone.snom;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.PhoneTestDriver;

public class SnomTest extends TestCase {

    private Phone m_phone;

    protected void setUp() {
        m_phone = new SnomPhone();
        PhoneTestDriver.supplyTestData(m_phone);
    }

  // Disabled until new sample config is ready
  //  public void testGenerateProfiles() throws Exception {
  //      StringWriter profile = new StringWriter();
  //      m_phone.generateProfile(profile);
  //      String expected = IOUtils.toString(this.getClass()
  //              .getResourceAsStream("expected-360.cfg"));
  //      assertEquals(expected, profile.toString());
  //  }

    public void testGetProfileName() {
        Phone phone = new SnomPhone();
        phone.setWebDirectory("web");
        // it can be called without serial number
        //assertEquals("web/snom360.htm", phone.getPhoneFilename());
        phone.setSerialNumber("abc123");
        assertEquals("web/ABC123.htm", phone.getPhoneFilename());
    }
}