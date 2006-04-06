#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

$SOURCE_DIR = File.dirname(__FILE__)    # directory in which this file is located

# system requirements
require 'parsedate'
require File.join($SOURCE_DIR, '..', 'test_helper')

# application requirements
require File.join($SOURCE_DIR, '..', '..', 'call_resolver')


class CallDirectionPluginTest < Test::Unit::TestCase
  fixtures :call_state_events, :parties, :cdrs
 
public

  def setup
    # Create the CallResolver, giving it the location of the test config file.
    @resolver = CallResolver.new(File.join($SOURCE_DIR, 'data/callresolver-config'))
    @plugin = CallDirectionPlugin.new(@resolver)
  end

  def test_set_call_direction_config
    # Pass in an empty config, should get the default value of false
    assert(!CallDirectionPlugin.call_direction?({}))
    
    # Pass in ENABLE, get true
    # Pass in DISABLE, get false
    # Comparison is case-insensitive
    assert(CallDirectionPlugin.call_direction?(
      {CallDirectionPlugin::CALL_DIRECTION => 'EnAbLe'}))
    assert(!CallDirectionPlugin.call_direction?(
      {CallDirectionPlugin::CALL_DIRECTION => 'dIsAbLe'}))
    
    # Pass in bogus value, get exception
    assert_raise(ConfigException) do
      CallDirectionPlugin.call_direction?(
        {CallDirectionPlugin::CALL_DIRECTION => 'jacket'})
    end
  end
  
  def test_call_direction
    # Clear out the gateways and set up a single test gateway
    @plugin.send(:gateways=, [Gateway.new(:address => "1.1.1.1")])
    
    # Resolve calls, computing call direction via the plugin
    Cdr.delete_all
    start_time = Time.parse('2001-1-1T00:00:00.000Z')
    end_time = Time.parse('2001-1-1T20:01:00.000Z')
    @resolver.add_observer(@plugin)
    @resolver.resolve(start_time, end_time)
    assert_equal(3, Cdr.count, 'Wrong number of CDRs')
    
    # Check that call direction came out as expected
    cdrs = Cdr.find(:all)
    assert_equal('I', cdrs[0].call_direction)
    assert_equal('O', cdrs[1].call_direction)
    assert_equal('A', cdrs[2].call_direction)
  end
  
end
