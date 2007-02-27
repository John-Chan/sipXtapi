#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

require 'utils/utils'

class CallLeg
  attr_reader :id, :connect_time, :end_time, :status, :to_tag, :failure_status, :failure_reason, :callee_contact
  
  def initialize(id)
    @id = id
    @status = nil
    @connect_time = nil
    @end_time = nil
    @failure_status = nil
    @failure_reason = nil
  end
  
  def has_duration?
    @connect_time && @end_time
  end
  
  def completed?
    @status == Cdr::CALL_COMPLETED_TERM
  end
  
  def duration
    raise ArgumentError, "only defined for completed" unless has_duration?
    @end_time - @connect_time
  end
  
  def accept_setup(cse)
    @connect_time = cse.event_time
    @status ||= Cdr::CALL_IN_PROGRESS_TERM
    @to_tag = cse.to_tag
    @callee_contact = Utils.contact_without_params(cse.contact)
  end
  
  def accept_end(cse)
    @end_time = cse.event_time
    @status = if cse.call_end? 
      Cdr::CALL_COMPLETED_TERM 
    else 
      @failure_reason = cse.failure_reason
      @failure_status = cse.failure_status
      Cdr::CALL_FAILED_TERM 
    end
    @to_tag ||= cse.to_tag
  end  
  
  include Comparable
  def <=>(other)
    # leg that has duration is always better than the one that does now
    return 1 if has_duration? && !other.has_duration?
    return -1 if !has_duration? && other.has_duration?
    if has_duration? and other.has_duration? 
      if @status == other.status 
        return duration <=> other.duration
      else
        # different status
        return  completed? ? 1 : -1
      end
    end
    
    # if we are here none has duration
    if @status == other.status
      # the same status
      return other.connect_time <=> @connect_time if @status == Cdr::CALL_IN_PROGRESS_TERM
      return @end_time <=> other.end_time
    else
      # different status
      return 1 if completed?
      return -1 if other.completed?
      return @status == Cdr::CALL_FAILED_TERM ? 1 : -1
    end
  end
  
  class << self
    # Compute call leg ID from 'from' and 'to' tags
    def leg_id(cse)
      from = cse.from_tag
      to = cse.to_tag
      if from < to
        "#{from}<>#{to}"
      else
        "#{to}<>#{from}"
      end        
    end    
  end
end


class CallLegs
  attr_reader :best_leg
  
  def initialize
    @legs = {}
    @best_leg = nil
  end
  
  def accept_setup(cse)
    leg = get_leg(cse)
    leg.accept_setup(cse)
    check_best_leg(leg)
  end
  
  def accept_end(cse)
    leg = get_leg(cse)
    leg.accept_end(cse)
    check_best_leg(leg)
  end
  
  def check_best_leg(leg)
    if !@best_leg || @best_leg < leg
      @best_leg = leg                
    end
    return @best_leg
  end
  
  def done?
    @best_leg && @best_leg.status != Cdr::CALL_IN_PROGRESS_TERM
  end
  
  private
  def get_leg(cse)
    id = CallLeg.leg_id(cse)
    @legs[id] ||= CallLeg.new(id)
  end
end

#
#  CDR description
#  id                                 primary key
#  call_id text not null              SIP call ID from the INVITE 
#  from_tag text not null             SIP from tag provided by caller in the INVITE 
#  to_tag text not null               SIP to tag provided by callee in the 200 OK 
#  caller_aor text not null           caller's SIP address of record (AOR) 
#  callee_aor text not null           callee's AOR 
#  start_time timestamp               Start time in GMT: initial INVITE received 
#  connect_time timestamp             Connect time in GMT: ACK received for 200 OK 
#  end_time timestamp                 End time in GMT: BYE received  or other ending 
#  termination char(1)                Why the call was terminated 
#  failure_status int2                SIP error code if the call failed  e.g.  4xx 
#  failure_reason text                Text describing the reason for a call failure 
#  call_direction char(1)             Plugin feature  see below 
class Cdr
  # Constants representing termination codes
  CALL_REQUESTED_TERM   = 'R'
  CALL_IN_PROGRESS_TERM = 'I'
  CALL_COMPLETED_TERM   = 'C'
  CALL_FAILED_TERM      = 'F'
  
  def initialize(call_id)
    @call_id = call_id
    @from_tag = nil
    @to_tag = nil
    @start_time = nil
    @connect_time = nil
    @end_time = nil    
    @termination = nil
    @failure_status = nil
    @failure_reason = nil
    @call_direction = nil
    
    @got_original = false
    @legs = CallLegs.new
    
    @callee_contact = nil
    @caller_contact = nil
  end
  
  FIELDS = [:call_id, :from_tag, :to_tag, :caller_aor, :callee_aor, 
  :start_time, :connect_time, :end_time,    
  :termination, :failure_status, :failure_reason, 
  :call_direction]
  
  attr_accessor(*FIELDS)
  
  # used to compute call direction
  attr_accessor :callee_contact, :caller_contact
  
  # Return true if the CDR is complete, false otherwise.
  def complete?
    @termination == CALL_COMPLETED_TERM || @termination == CALL_FAILED_TERM
  end
  
  def terminated?
    @termination == CALL_COMPLETED_TERM
  end
  
  # Return a text description of the termination status for this CDR.
  def termination_text
    TERMINATION_NAMES[@termination] if @termination
  end  
  
  def accept(cse)
    case
    when cse.call_request?
      accept_call_request(cse)
    when cse.call_setup?
      accept_call_setup(cse)
    when cse.call_end?, cse.call_failure?
      accept_call_end(cse)
    end
  end
  
  # called when we are done with processing of this CDR
  # cleans temporry data structures
  def retire
    @legs = nil
  end
  
  def to_s
    "CDR: #{@call_id}, from #{@caller_aor} to #{@callee_aor} status #{@termination}"
  end
  
  private
  
  # original (without to_tag) request is always better than not original (with to_tag) request
  def accept_call_request(cse)
    original = !cse.to_tag
    # bailout if we already have original request and this one is not
    return if(@got_original && !orginal) 
    
    # continue if we are original or if we are older 
    if((!@got_original && original) || !@start_time || @start_time > cse.event_time)
      
      @from_tag = cse.from_tag
      @caller_aor = cse.caller_aor
      @callee_aor = cse.callee_aor
      @start_time = cse.event_time      
      @termination = CALL_REQUESTED_TERM unless @termination
      
      @caller_contact = Utils.contact_without_params(cse.contact)
      
      @got_original ||= !cse.to_tag      
    end
    return nil
  end
  
  def accept_call_setup(cse)
    leg = @legs.accept_setup(cse)
    finish
  end
  
  def accept_call_end(cse)
    leg = @legs.accept_end(cse)
    finish
  end
  
  def finish
    return unless @legs.done?
    leg = @legs.best_leg
    @to_tag = leg.to_tag
    @connect_time = leg.connect_time
    @end_time = leg.end_time
    @termination = leg.status
    @failure_reason = leg.failure_reason
    @failure_status = leg.failure_status
    @callee_contact = leg.callee_contact
    return self
  end
  
  # Map termination codes to human-readable strings
  TERMINATION_NAMES = { 
    CALL_REQUESTED_TERM   => 'requested',
    CALL_IN_PROGRESS_TERM => 'in progress',
    CALL_COMPLETED_TERM   => 'completed',
    CALL_FAILED_TERM      => 'failed'}  
end