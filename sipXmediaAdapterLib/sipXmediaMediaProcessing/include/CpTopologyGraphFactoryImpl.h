//
// Copyright (C) 2007-2009 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2007-2009 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Dan Petrie (dpetrie AT SIPez DOT com)

#ifndef _CpTopologyGraphFactoryImpl_h_
#define _CpTopologyGraphFactoryImpl_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include <sipXmediaFactoryImpl.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class MpResourceTopology;
class MpResourceFactory;
class MpInputDeviceManager;
class MpOutputDeviceManager;
class MpMMTimer;

/** 
*  @brief Subsystem manager and creator of CpTopologyGraphInterfaces
*         specialization of CpMediaInterface.
*
*  When createMediaInterface is invoked a new CpTopologyGraph (flowgraph)
*  is constructed and used inside the CpTopologyGraphInterface (CpMediaInterface).
*  The topology and types of the resources in the new flowgraph are determined
*  by the MpResourceTopology last set via setInitialResourceTopology.  The
*  specific resources are instantiated using the MpResourceFactory last set
*  via setResourceFactory.
*
*  @nosubgrouping
*/
class CpTopologyGraphFactoryImpl : public sipXmediaFactoryImpl
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

      /// @copydoc sipXmediaFactoryImpl::sipXmediaFactoryImpl()
    CpTopologyGraphFactoryImpl(OsConfigDb* pConfigDb,
                               uint32_t frameSizeMs, 
                               uint32_t maxSamplesPerSec,
                               uint32_t defaultDeviceSamplesPerSec,
                               UtlBoolean enableLocalAudio,
                               const UtlString &inputDeviceName,
                               const UtlString &outputDeviceName);

      /// Destructor.
    virtual
    ~CpTopologyGraphFactoryImpl();

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

      /// @brief Factory method to construct new flowgraph and media interface
      /// using CpTopologyFlowgraph.
    virtual
    CpMediaInterface* createMediaInterface(const char* publicAddress,
                                           const char* localAddress,
                                           int numCodecs,
                                           SdpCodec* sdpCodecArray[],
                                           const char* locale,
                                           int expeditedIpTos,
                                           const char* stunServer,
                                           int stunOptions,
                                           int stunKeepAliveSecs,
                                           const char* turnServer,
                                           int turnPort,
                                           const char* turnUserName,
                                           const char* turnPassword,
                                           int turnKeepAlivePeriodSecs,
                                           UtlBoolean enableIce, 
                                           uint32_t samplesPerSec,
                                           OsMsgDispatcher* pDispatcher);

      /// @copydoc CpMediaInterfaceFactoryImpl::setSpeakerDevice()
    virtual OsStatus setSpeakerDevice(const UtlString& device);
      /// @copydoc CpMediaInterfaceFactoryImpl::setMicrophoneDevice()
    virtual OsStatus setMicrophoneDevice(const UtlString& device);
      /// @copydoc CpMediaInterfaceFactoryImpl::setAudioAECMode()
    virtual OsStatus setAudioAECMode(const MEDIA_AEC_MODE mode);
      /// @copydoc CpMediaInterfaceFactoryImpl::enableAGC()
    virtual OsStatus enableAGC(UtlBoolean bEnable);
      /// @copydoc CpMediaInterfaceFactoryImpl::setAudioNoiseReductionMode()
    virtual OsStatus setAudioNoiseReductionMode(const MEDIA_NOISE_REDUCTION_MODE mode);

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

      /// @copydoc CpMediaInterfaceFactoryImpl::getSpeakerDevice()
    virtual OsStatus getSpeakerDevice(UtlString& device) const;
      /// @copydoc CpMediaInterfaceFactoryImpl::getMicrophoneDevice()
    virtual OsStatus getMicrophoneDevice(UtlString& device) const;

      /// Set the resource topology to be used when creating new flowgraph.
    virtual
    void setInitialResourceTopology(MpResourceTopology& resourceTopology);
      /**<
      *  The given CpTopolgyGraph defines the topology and types of resources
      *  that are instantiated in the new flowgraph.
      */

      /// @brief Get the resource topology defining the resources connections
      /// for a new flowgraph.
    virtual
    MpResourceTopology* getInitialResourceTopology() const;

      /// Set the factory used to create resources for the new flowgraph contained .
    virtual
    void setResourceFactory(MpResourceFactory& resourceFactory);

      /// Get the factory for constructing new resources.
    virtual
    MpResourceFactory* getResourceFactory() const;

      /// @brief Set the resource topology to be added to the flow graph when
      /// adding an unicast RTP connection.
    virtual
    void setConnectionResourceTopology(MpResourceTopology& connectionTopology);

      /// @brief Get the resource topology defining what resources get added
      /// when adding an unicast RTP connection.
    virtual
    MpResourceTopology* getConnectionResourceTopology() const;

      /// @brief Set the resource topology to be added to the flow graph when
      /// adding an multicast RTP connection.
    virtual
    void setMcastConnectionResourceTopology(MpResourceTopology& connectionTopology);

      /// @brief Get the resource topology defining what resources get added
      /// when adding an multicast RTP connection.
    virtual
    MpResourceTopology* getMcastConnectionResourceTopology() const;

      /// Return number of RTP streams multicast connection supports
    inline
    int getNumMcastRtpStreams() const;

    MpInputDeviceManager* getInputDeviceManager() const;

      /// Build a resource factory with the default set of resource constructors.
    MpResourceFactory* buildDefaultResourceFactory();

      /// Build a default initial resource topology.
    static
    MpResourceTopology* buildDefaultInitialResourceTopology();

      /// Build an incremental resource topology for unicast RTP connections.
    MpResourceTopology* buildUnicastConnectionResourceTopology();

      /// Build an incremental resource topology for multicast RTP connections.
    MpResourceTopology* buildMulticastConnectionResourceTopology();

      /// Get number of input connections this topology can support.
    static int getMaxInputConnections();

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

   MpResourceTopology    *mpInitialResourceTopology;
   MpResourceFactory     *mpResourceFactory;
   MpResourceTopology    *mpConnectionResourceTopology;
   MpResourceTopology    *mpMcastConnectionResourceTopology;
   UtlBoolean             mIsLocalAudioEnabled;
   MpInputDeviceManager  *mpInputDeviceManager;
   MpOutputDeviceManager *mpOutputDeviceManager;
   MpMMTimer             *mpMediaTaskTicker;
   int                    mNumMcastStreams;

     /// Add RTP output connection to topology
   static void addOutputConnectionTopology(MpResourceTopology* resourceTopology,
                                           int logicalPortNum);

     /// Add local input and local output connections to topology
   static void addLocalConnectionTopology(MpResourceTopology* resourceTopology);

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

};

/* ============================ INLINE METHODS ============================ */

int CpTopologyGraphFactoryImpl::getNumMcastRtpStreams() const
{
   return mNumMcastStreams;
}

#endif  // _CpTopologyGraphFactoryImpl_h_
