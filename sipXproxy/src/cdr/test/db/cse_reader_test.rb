#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

require 'test/unit'

$:.unshift(File.join(File.dirname(__FILE__), '..', '..', 'lib'))

require 'db/database_url'
require 'db/cse_reader'

class CseReaderTest < Test::Unit::TestCase
  def test_select_sql
    sql = CseReader.select_sql
    assert_equal("SELECT id, observer, event_seq, event_time, event_type, cseq, " +
      "call_id, from_tag, to_tag, " +
      "from_url, to_url, contact, refer_to, referred_by, failure_status, " +
      "failure_reason, request_uri FROM call_state_events " +
      "ORDER BY event_time", sql)
    
    sql = CseReader.select_sql(Time.now, nil)
    assert_equal("SELECT id, observer, event_seq, event_time, event_type, cseq, " +
      "call_id, from_tag, to_tag, " +
      "from_url, to_url, contact, refer_to, referred_by, failure_status, " +
      "failure_reason, request_uri FROM call_state_events " +
      "WHERE event_time >= ? " +
      "ORDER BY event_time", sql)    
    
    sql = CseReader.select_sql(nil, Time.now)
    assert_equal("SELECT id, observer, event_seq, event_time, event_type, cseq, " +
      "call_id, from_tag, to_tag, " +
      "from_url, to_url, contact, refer_to, referred_by, failure_status, " +
      "failure_reason, request_uri FROM call_state_events " +
      "WHERE event_time <= ? " +
      "ORDER BY event_time", sql)    
    
    sql = CseReader.select_sql(Time.new, Time.new)
    assert_equal("SELECT id, observer, event_seq, event_time, event_type, cseq, " +
      "call_id, from_tag, to_tag, " +
      "from_url, to_url, contact, refer_to, referred_by, failure_status, " +
      "failure_reason, request_uri FROM call_state_events " + 
      "WHERE event_time >= ? AND event_time <= ? " +
      "ORDER BY event_time", sql)
  end
  
  def test_delete_sql
    sql = CseReader.delete_sql(Time.new, Time.new)
    assert_equal("DELETE FROM call_state_events " + 
      "WHERE event_time >= ? AND event_time <= ?", sql)    
  end
  
end
