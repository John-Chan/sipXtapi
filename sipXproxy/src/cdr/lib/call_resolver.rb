#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

require 'logger'

require 'call_direction/call_direction_plugin'
require 'db/cse_reader'
require 'db/cdr_writer'
require 'utils/configure'
require 'state'


# The CallResolver analyzes call state events (CSEs) and computes call detail 
# records (CDRs).  It loads CSEs from a database and writes CDRs back into the
# same database.
class CallResolver
  attr_reader :log
  
  def initialize(config)
    @config =  config
    @log = config.log
    urls = @config.cse_database_urls
    purge_age_cse =  @config.purge_age_cse
    # readers put events in CSE queue
    @readers = urls.collect do | url |
      CseReader.new(url, purge_age_cse, log)
    end
    install_signal_handler(@readers)    
    @writer = CdrWriter.new(@config.cdr_database_url, @config.purge_age_cdr, log)
  end
  
  def run_resolver
    resolve(@writer.last_cdr_start_time, nil)
  end
  
  # Resolve CSEs to CDRs
  def resolve(start_time, end_time)
    start_run = Time.now
    
    # start everything    
    
    cdr_queue = Queue.new
    cse_queue = Queue.new    
    
    reader_threads = @readers.collect do | reader |
      Thread.new(reader, cse_queue) { |r, q| r.run(q, start_time, end_time) }
    end
    
    Thread.new( cse_queue, cdr_queue ) { | inq, outq | 
      # state copies from CSE queue to CDR queue
      State.new( cse_queue, cdr_queue ).run
    }
    
    # FIXME: enable call direction plugin
    #cdr_queue = start_plugins(cdr_queue)
    
    writer_thread = Thread.new( @writer, cdr_queue ) { | w, q | w.run(q) }    
    
    reader_threads.each{ |thread| thread.join }
    
    # send sentinel event, it will stop plugins and state
    cse_queue.enq(nil)
    
    # wait for writer before exiting
    writer_thread.join
    log.info("resolve: Done. Analysis took #{Time.now - start_run} seconds.")
  end
  
  
  # install handler for INT (2) and TERM (9) signals to cleanly terminate readers threads
  def install_signal_handler(readers)
    %w( TERM INT ).each do | s |
      Signal.trap(s) do
        log.info("#{s} intercepted. Terminating reader threads.")
        readers.each { |r| r.stop() }
      end        
    end
  end  
  
  def start_plugins(raw_queue)
    return raw_queue unless CallDirectionPlugin.call_direction?(@config)    
    processed_queue = Queue.new
    
    cdp = CallDirectionPlugin.new(raw_queu, processed_queue)  
    Thread.new(cdp) { | plugin | plugin.run }
    
    return cdr_queue
  end  
end
