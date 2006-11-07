#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

require 'test/unit'
require 'time'
require 'dbi'

$:.unshift File.join(File.dirname(__FILE__), "..", "lib")
require 'cdr'
require 'call_state_event'

$:.unshift File.join(File.dirname(__FILE__), "..", "test")
require 'test_util'
include TestUtil

def make_cse(args)
  cse = CallStateEvent.new
  # some defaults
  cse.event_seq= 1
  #  cse.event_time=DBI::Timestamp.new(Time.parse('1990-05-17T19:30:00.000Z'))
  cse.event_time=Time.parse('1990-05-17T19:30:00.000Z')
  cse.event_type='R'
  cse.cseq=1
  cse.call_id='test'
  cse.from_tag="abcd"
  cse.to_tag="efghss"
  cse.from_url='sip:alice@example.com; tag=f'
  cse.to_url='sip:bob@example.com'
  cse.contact='sip:alice@1.1.1.1'
  
  if(args)
    args.each { |field, value| 
      setter = (field.to_s + "=").to_sym
      cse.send( setter, value ) 
    }
  end
  
  return cse 
end

class CallLegTest < Test::Unit::TestCase
  
  def test_call_leg_id
    cse1 = make_cse(:from_tag => "aaa", :to_tag => "bbb")
    cse2 = make_cse(:from_tag => "bbb", :to_tag => "aaa")
    assert_equal(CallLeg.leg_id(cse1), CallLeg.leg_id(cse2))
    cse3 = make_cse(:from_tag => "xbbb", :to_tag => "aaa")
    assert_not_equal(CallLeg.leg_id(cse1), CallLeg.leg_id(cse3))    
    assert_not_equal(CallLeg.leg_id(cse2), CallLeg.leg_id(cse3))    
  end
  
  def test_has_duration?
    l = CallLeg.new('to')
    assert !l.has_duration?
    
    cse_setup = make_cse(:event_time => 100, :event_type => "S")
    l.accept_setup(cse_setup)    
    assert !l.has_duration?
    
    cse_end = make_cse(:event_time => 200, :event_type => "F")
    l.accept_end(cse_setup)
    assert l.has_duration?
    
    l = CallLeg.new('to')
    cse_end = make_cse(:event_time => 200, :event_type => "E")
    l.accept_end(cse_setup)
    assert !l.has_duration?
    
    cse_setup = make_cse(:event_time => 100, :event_type => "S")
    l.accept_setup(cse_setup)        
    assert l.has_duration?
  end
  
  def test_compare
    l1 = CallLeg.new('to1')
    l2 = CallLeg.new('to2')
    
    cse_setup = make_cse(:event_time => 100, :event_type => "S")
    l1.accept_setup(cse_setup)
    l2.accept_setup(cse_setup)
    cse_end = make_cse(:event_time => 200, :event_type => "E")
    l1.accept_end(cse_end)
    cse_failure = make_cse(:event_time => 200, :event_type => "F")
    l2.accept_end(cse_failure)    
    assert l1 > l2
    
    
    l2 = CallLeg.new('to_2')    
    l2.accept_setup(cse_setup)
    l2.accept_end(make_cse(:event_time => 250, :event_type => "F"))
    assert l1 > l2
    
    l2 = CallLeg.new('to_2')    
    l2.accept_setup(cse_setup)
    l2.accept_end(make_cse(:event_time => 250, :event_type => "E"))
    assert l1 < l2
  end
  
  def test_compare_equals
    l1 = CallLeg.new('to1')
    l2 = CallLeg.new('to2')
    
    cse_setup = make_cse(:event_time => 100, :event_type => "S")
    cse_end = make_cse(:event_time => 250, :event_type => "E")
    
    l1.accept_setup(cse_setup)
    l2.accept_setup(cse_setup)
    l1.accept_end(cse_end)
    l2.accept_end(cse_end)
    
    assert l1 = l2
    
  end
end

class CallLegsTest < Test::Unit::TestCase
  
  def test_empty_done?
    assert !CallLegs.new.done?
  end
  
  def test_accept
    cl = CallLegs.new
    
    cse = make_cse(:event_type => 'S', :event_time => 100)
    assert_not_nil cl.accept_setup(cse)    
    assert_not_nil cl.best_leg    
    
    cse = make_cse(:event_type => 'F', :event_time => 200)
    leg = cl.accept_end(cse)    
    assert_kind_of(CallLeg, leg)
    
    assert_equal(100, leg.duration)
    
    assert_same(leg, cl.best_leg)
    
    assert cl.done? 
  end
  
  
  def test_accept_many
    cl = CallLegs.new
    
    cse = make_cse(:to_tag => 't5', :event_type => 'S', :event_time => 100)
    assert_not_nil cl.accept_setup(cse)    
    assert_not_nil cl.best_leg    
    assert !cl.done? 
    
    cse = make_cse(:to_tag => 't6', :event_type => 'S', :event_time => 101)
    assert_not_nil cl.best_leg    
    assert !cl.done? 
    
    cse = make_cse(:to_tag => 't6', :event_type => 'F', :event_time => 200)
    leg = cl.accept_end(cse)    
    assert_kind_of(CallLeg, leg)
    assert_same(leg, cl.best_leg)
    assert cl.done? 
    
    cse = make_cse(:to_tag => 't5', :event_type => 'E', :event_time => 201)
    leg = cl.accept_end(cse)    
    assert_kind_of(CallLeg, leg)
    
    assert_equal(101, leg.duration)
    
    assert_same(leg, cl.best_leg)
    
    assert cl.done? 
  end
  
  def test_accept_out_of_order
    cl = CallLegs.new
    
    cse = make_cse(:from_tag => 't1', :to_tag => 't6', :event_type => 'F', :event_time => 200)
    leg = cl.accept_end(cse)    
    assert_not_nil cl.best_leg    
    assert cl.done? 
    
    cse = make_cse(:to_tag => 't5', :event_type => 'S', :event_time => 101)
    assert_not_nil cl.accept_setup(cse)    
    assert_not_nil cl.best_leg    
    assert cl.done? 
    
    cse = make_cse(:from_tag => 't6', :to_tag => 't1', :event_type => 'S', :event_time => 100)
    leg = cl.accept_setup(cse)    
    assert_kind_of(CallLeg, leg)
    assert_same(leg, cl.best_leg)
    assert cl.done? 
    
    cse = make_cse(:to_tag => 't5', :event_type => 'E', :event_time => 201)
    leg = cl.accept_end(cse)    
    assert_kind_of(CallLeg, leg)
    
    assert_equal(100, leg.duration)
    
    assert_same(leg, cl.best_leg)
    
    assert cl.done? 
  end
end


class CdrTest < Test::Unit::TestCase
  def test_accept_request
    cdr = Cdr.new('test')
    cse = make_cse(:to_tag => nil, :event_type => 'R', :event_time => 100)
    assert_nil cdr.accept(cse)    
  end
  
  def test_accept_setup
    cdr = Cdr.new('test')
    cse = make_cse(:to_tag => 't7', :event_type => 'S', :event_time => 100)
    assert_nil cdr.accept(cse)    
  end
  
  def test_accept_failure
    cdr = Cdr.new('test')
    cse = make_cse(:to_tag => 't6', :event_type => 'F', :event_time => 100)
    assert_not_nil cdr.accept(cse)    
  end
  
  
  def test_accept_end
    cdr = Cdr.new('test')
    cse = make_cse(:to_tag => 't6', :event_type => 'E', :event_time => 100)
    assert_not_nil cdr.accept(cse)    
  end
  
  
  def test_accept_transfer
    cdr = Cdr.new('test')
    cse = make_cse(:to_tag => 't6', :event_type => 'T', :event_time => 100)
    assert_nil cdr.accept(cse)    
  end
  
  def test_call_success
    cdr = Cdr.new('test')
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'R')
    assert_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'S')
    assert_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'E')
    assert_not_nil cdr.accept(cse)
    assert_equal('t1', cdr.from_tag)
    assert_equal('t7', cdr.to_tag)
  end
  
  def test_call_failed
    cdr = Cdr.new('test')
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'R')
    assert_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'F')
    assert_not_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'S')
    assert_not_nil cdr.accept(cse)
    assert_equal('t1', cdr.from_tag)
    assert_equal('t7', cdr.to_tag)
  end
  
  def test_call_failed_no_setup
    cdr = Cdr.new('test')
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'R')
    assert_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'F')
    assert_not_nil cdr.accept(cse)    
    assert_equal('t1', cdr.from_tag)
    assert_equal('t7', cdr.to_tag)
  end
  
  
  def test_call_reversed_tags
    cdr = Cdr.new('test')
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'R')
    assert_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't7', :to_tag => 't1', :event_type => 'F')
    assert_not_nil cdr.accept(cse)    
    cse = make_cse(:from_tag => 't1', :to_tag => 't7', :event_type => 'S')
    assert_not_nil cdr.accept(cse)
    assert_equal('t1', cdr.from_tag)
    assert_equal('t7', cdr.to_tag)
  end
  
  def test_complete?
    cdr = Cdr.new('call_ui')
    
    # No termination code => CDR is not complete
    assert(!cdr.complete?)
    
    # Termination code of completion or failure implies complete CDR
    cdr.termination = Cdr::CALL_COMPLETED_TERM
    assert(cdr.complete?)
    cdr.termination = Cdr::CALL_FAILED_TERM
    assert(cdr.complete?)
    
    # Termination code of request or in progress implies incomplete CDR
    cdr.termination = Cdr::CALL_REQUESTED_TERM
    assert(!cdr.complete?)
    cdr.termination = Cdr::CALL_IN_PROGRESS_TERM
    assert(!cdr.complete?)
  end
  
end
