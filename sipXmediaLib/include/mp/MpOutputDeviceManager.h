//  
// Copyright (C) 2007 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Alexander Chemeris <Alexander DOT Chemeris AT SIPez DOT com>

#ifndef _MpOutputDeviceManager_h_
#define _MpOutputDeviceManager_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include <os/OsRWMutex.h>
//#include <utl/UtlHashMap.h>
//#include <utl/UtlHashBag.h>
#include "mp/MpTypes.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class MpOutputDeviceDriver;
class UtlString;
class MpBufPtr;

/**
*  @brief Container of output devices.
*
*  The MpOutputDeviceManager is a container of output device drivers, serving
*  as sink for MprToOuputDevice resources. MpOutputDeviceDriver instances are
*  added to the MpOutputDeviceManager and removed from it by external entities,
*  using addDevice() and removeDevice(). Device drivers may be used in two modes:
*  direct write mode and mixer mode. See MpOutputDeviceDriver description for
*  for detailed description of this modes.
*  The specific device driver is accessed by the device ID (handle).
*  The MpOutputDeviceManager maintains a device ID to device name mapping. All
*  device IDs and device names are unique within the scope of this
*  MpOutputDeviceManager.  The device ID provides a level of indirection that
*  allows the MpOutputDeviceManager to protect against access of transient
*  devices.
*
*  MpOutputDeviceManager allows multiple MprToOutputDevice resources to
*  send a frame of media via the pushFrame() method. 
*  
*  The MpOutputDeviceManager uses MpAudioOutputConnection internally to
*  map the association between MpOutputDeviceDriver and device ID.  The
*  MpAudioInputConnection also maintains the mixer buffer when used in
*  mixer mode.
*
*  @NOTE This class is intensionally NOT a singleton so that multiple
*        instances can be created in support of multiple instances of the media
*        subsystem being created.  Multiple instances may be useful for taking
*        advantage of multi-processors, testing and other interesting applications.
*/
class MpOutputDeviceManager
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// @brief Default constructor.
   MpOutputDeviceManager(unsigned defaultSamplesPerFrame, 
                         unsigned defaultSamplesPerSecond,
                         MpFrameTime defaultMixerBufferLength);
     /**<
     *  @param defaultSamplesPerFrame - (in) the default number of samples in
     *         media frame. Will be used when enabling devices.
     *  @param defaultSamplesPerSecond - (in) default sample rate for media frame
     *         in samples per second. Will be used when enabling devices.
     *  @param defaultMixerBufferLength - (in) default length of mixer buffer in
     *         milliseconds. Will be used when enabling devices. Mixer buffer
     *         is used when direct write mode is disabled to mix multiple media
     *         streams. Stream sources (callers of pushFrame()) should produce
     *         data with difference in time less, then mixer buffer length,
     *         delayed stream data would be rejected. Set <tt>defaultBufferLength</tt>
     *         to 0 to enable direct write mode by default.
     */


     /// @brief Destructor.
   virtual
   ~MpOutputDeviceManager();
     /**<
     *  @NOTE This is NOT thread safe.  The invoker of this destructor
     *        MUST be sure that no device drivers or resources are referencing
     *        this device manager.
     */

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

     /// @brief Add a new input device for use.
   MpOutputDeviceHandle addDevice(MpOutputDeviceDriver& newDevice);
     /**<
     *  Returns device ID which is unique within this device manager.
     *  This method locks the device manager for exclusive use.
     *
     *  @param newDevice - (in) A new input device to add to this manager.
     *                     The device should <b>only</b> be added to one manager.
     *
     *  @returns A handle to reference the device by in other manager calls.
     *
     *  Multi-thread safe.
     */


     /// @brief Remove an existing input device.
   MpOutputDeviceDriver* removeDevice(MpOutputDeviceHandle deviceId);
     /**<
     *  This method locks the device manager for exclusive use.
     *
     *  @param deviceId - (in) The device to disable.
     *
     *  @returns A pointer to the input device driver that was removed.
     *  @returns NULL if the device could not be found or device is busy.
     *
     *  Multi-thread safe.
     */


     /// @brief Helper to enable device driver.
   OsStatus enableDevice(MpOutputDeviceHandle deviceId,
                         MpFrameTime mixerBufferLength);
     /**<
     *  This method enables the device driver indicated by the device id.
     *
     *  @param deviceId - (in) The device to enable.
     *  @param mixerBufferLength - (in) length of mixer buffer in milliseconds.
     *         Mixer buffer is used when direct write mode is disabled to mix
     *         multiple media streams. Stream sources (callers of pushFrame())
     *         should produce data with difference in time less, then mixer
     *         buffer length, delayed stream data would be rejected.
     *         Set <tt>mixerBufferLength</tt> to 0 to enable direct write mode.
     *
     *  @returns OS_NOT_FOUND if the device could not be found.
     *  
     *  @NOTE This SHOULD NOT be used to mute/unmute a device. Disabling and
     *        enabling a device results in state and buffer queues being cleared.
     *
     *  Multi-thread safe.
     */


     /// @brief Helper to disable device driver.
   OsStatus disableDevice(MpOutputDeviceHandle deviceId);
     /**<
     *  This method disables the device driver indicated by the device id.
     *
     *  @param deviceId - (in) The device to disable.
     *
     *  @returns OS_NOT_FOUND if the device could not be found.
     *  @returns OS_BUSY if the device is currently being removed or disabled.
     *
     *  @NOTE This SHOULD NOT be used to mute/unmute a device. Disabling and
     *        enabling a device results in state and buffer queues being cleared.
     *
     *  Multi-thread safe.
     */


     /// @brief Method for sending frame of data to output device.
   OsStatus pushFrame(MpOutputDeviceHandle deviceId,
                      MpFrameTime frameTime,
                      const MpBufPtr& frame,
                      MpFrameTime& alreadyBuffered);
     /**<
     *  This method is used to push a frame to the MpOutputDeviceManager to be
     *  buffered for a short window of time and mixed with data from other
     *  contributors.
     *  This method is typically invoked by MprToOutputDevice resources.
     *
     *  @param deviceId - (in) device id to identify device to which this frame
     *         will be sent.
     *  @param frameTime - (in) time in milliseconds for beginning of frame
     *         relative to the MpOutputDeviceManager reference time.
     *  @param frame - (in) frame of media to be sent to output device.
     *  @param alreadyBuffered - (out) length of data (in milliseconds) stored
     *         in mixer buffer and not yet send to device driver.
     *
     *  @returns OS_NOT_FOUND if the device could not be found.
     *  @returns OS_LIMIT_REACHED if mixer buffer is full.
     *
     *  Multi-thread safe.
     */


     /// @brief Method for MpOutputDeviceDriver to get data from mixer buffer.
   OsStatus pullFrame(MpOutputDeviceHandle deviceId,
                      MpFrameTime frameTime
                      unsigned numSamples,
                      MpAudioSample *samples);
     /**<
     *  Method for pulling next frame of media data for playback.
     *  This method is typically invoked by MpOutputDeviceDriver objects in
     *  non direct write mode.
     *
     *  @param deviceId - (in) device id to identify device which pull the data.
     *  @param frameTime - (in) time in milliseconds for beginning of frame
     *         relative to the MpOutputDeviceManager reference time.
     *  @param numSamples - (in) number of samples in the frame of media to
     *         pull from mixer buffer.
     *  @param samples - (in) pointer to buffer for the the frame data.
     *
     *  @returns OS_NOT_FOUND if the device id does not exist.
     *  @returns OS_INVALID_STATE if device is configured for direct write
     *                            operation and mixer buffer is not available.
     *  @returns OS_INVALID_ARGUMENT if the frame for the requested time is not
     *                               available.
     *
     *  Multi-thread safe.
     */

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

     /// @brief Get the device driver name for the given device ID
   OsStatus getDeviceName(MpOutputDeviceHandle deviceId, UtlString& deviceName) const;
     /**<
     *  Get the name for the given deviceId.
     *
     *  @param deviceId - (in) The device to fetch the name of.
     *  @param deviceName - (out) a UtlString that will hold the device name.
     *
     *  @returns OS_SUCCESS if device found and <tt>deviceName</tt> filled with
     *                      the name of the device.
     *  @returns OS_NOT_FOUND if specified device could not be found.
     *
     *  Multi-thread safe.
     */


     /// @brief Get the device id for the given device driver name
   OsStatus getDeviceId(const UtlString& deviceName, MpOutputDeviceHandle &deviceId) const;
     /**<
     *  The MpOutputDeviceManager maintains a device ID to device name
     *  mapping.  All device IDs and device names are unique within the
     *  scope of this MpOutputDeviceManager.
     *
     *  @param deviceName - (in) The name of a device to get the ID of.
     *  @param deviceId - (out) A handle to reference the device by in other
     *         manager calls.
     *
     *  @returns OS_SUCCESS if device found and <tt>deviceId</tt> filled in.
     *  @returns OS_NOT_FOUND if specified device could not be found.
     *
     *  Multi-thread safe.
     */


     /// @brief Get current frame timestamp
   MpFrameTime getCurrentFrameTime() const;
     /**<
     *  The timestamp is in milliseconds from the initial reference point
     *  in time for this device manager
     *
     *  @NOTE This number will wrap roughly every 49.7 days.
     *
     *  Multi-thread safe.
     */

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

     /// @brief Inquire if device is enabled (e.g. consuming media data).
   UtlBoolean isDeviceEnabled(MpOutputDeviceHandle deviceId);
     /**<
     *  Inquire if specified device is enabled (e.g. consuming media data).
     *
     *  @param deviceId - (in) The device to determine enabled status of.
     */

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:


/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

  OsRWMutex mRwMutex;
  MpOutputDeviceHandle mLastDeviceId;
  unsigned mDefaultSamplesPerFrame;
  unsigned mDefaultSamplesPerSecond;
  MpFrameTime mDefaultBufferLength;
  UtlHashMap mConnectionsByDeviceName;
  UtlHashBag mConnectionsByDeviceId;
  OsTime mTimeZero;

    /// Copy constructor (not implemented for this class)
  MpOutputDeviceManager(const MpOutputDeviceManager& rMpOutputDeviceManager);

    /// Assignment operator (not implemented for this class)
  MpOutputDeviceManager& operator=(const MpOutputDeviceManager& rhs);

};

/* ============================ INLINE METHODS ============================ */

#endif  // _MpOutputDeviceManager_h_
