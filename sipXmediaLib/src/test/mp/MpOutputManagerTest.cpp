//  
// Copyright (C) 2007-2010 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Alexander Chemeris <Alexander DOT Chemeris AT SIPez DOT com>

#include <os/OsIntTypes.h>

#include <sipxunittests.h>

#include <mp/MpOutputDeviceManager.h>
#include <mp/MpAudioBuf.h>
#include <mp/MpSineWaveGeneratorDeviceDriver.h>
#include <os/OsTask.h>
#include <os/OsEvent.h>

#define TEST_SAMPLES_PER_FRAME_SIZE   80    ///< in samples
#define BUFFER_NUM                    500
#define TEST_SAMPLES_PER_SECOND       8000  ///< in samples/sec (Hz)
#define TEST_MIXER_BUFFER_LENGTH      100   ///< in milliseconds
#define TEST_SAMPLE_FRAMES            100   ///< test length in frames
#define TEST_SAMPLE_DATA_MAGNITUDE    32000 ///< absolute value
#define TEST_SAMPLE_DATA_PERIOD       11    ///< in milliseconds
#define TEST_FRAME_LENGTH_MS          (1000/(TEST_SAMPLES_PER_SECOND/TEST_SAMPLES_PER_FRAME_SIZE))
                                            ///< in milliseconds

#define NONEXISTENT_DEVICE_ID         321
#define NONEXISTENT_DEVICE_NAME       "NONE"

#define CREATE_TEST_RUNS_NUMBER              3
#define NONEXISTENT_TEST_RUNS_NUMBER         3
#define ADD_REMOVE_TEST_RUNS_NUMBER          3
#define ENABLE_DISABLE_TEST_RUNS_NUMBER      5
#define ENABLE_DISABLE_FAST_TEST_RUNS_NUMBER 10
#define DIRECT_WRITE_TEST_RUNS_NUMBER        3
#define TICKER_TEST_WRITE_RUNS_NUMBER        3

#define USE_TEST_DRIVER

#ifdef USE_TEST_DRIVER // USE_TEST_DRIVER [
#include <mp/MpodBufferRecorder.h>
#define OUTPUT_DRIVER MpodBufferRecorder
#define OUTPUT_DRIVER_CONSTRUCTOR_PARAMS "default", TEST_SAMPLE_FRAMES*TEST_SAMPLES_PER_FRAME_SIZE*1000/TEST_SAMPLES_PER_SECOND

#elif defined(WIN32) // USE_TEST_DRIVER ][ WIN32
#include <mp/MpodWinMM.h>
#define OUTPUT_DRIVER MpodWinMM
#define OUTPUT_DRIVER_CONSTRUCTOR_PARAMS MpodWinMM::getDefaultDeviceName()

#elif defined(__pingtel_on_posix__) // WIN32 ][ __pingtel_on_posix__
#include <mp/MpodOss.h>
#define OUTPUT_DRIVER MpodOss
#define OUTPUT_DRIVER_CONSTRUCTOR_PARAMS "/dev/dsp"

#else // __pingtel_on_possix__ ]
#error Unknown platform!
#endif

static void calculateSampleData(MpFrameTime frameTime,
                                MpAudioBufPtr &frame)
{
   MpAudioSample *sampleData = frame->getSamplesWritePtr();
   for (unsigned i=0; i<frame->getSamplesNumber(); i++)
   {
      sampleData[i] = 
         MpSineWaveGeneratorDeviceDriver::calculateSample(frameTime,
                                                          TEST_SAMPLE_DATA_MAGNITUDE,
                                                          TEST_SAMPLE_DATA_PERIOD,
                                                          i,
                                                          TEST_SAMPLES_PER_FRAME_SIZE,
                                                          TEST_SAMPLES_PER_SECOND);
   }
}

/**
 * Unittest for MpOutputDeviceDriver
 */
class MpOutputDeviceManagerTest : public SIPX_UNIT_BASE_CLASS
{
   CPPUNIT_TEST_SUITE(MpOutputDeviceManagerTest);
   CPPUNIT_TEST(testCreate);
   CPPUNIT_TEST(testNonexistentDevice);
   CPPUNIT_TEST(testAddRemoveToManager);
   CPPUNIT_TEST(testEnableDisable);
   CPPUNIT_TEST(testEnableDisableFast);
   CPPUNIT_TEST(testMixing);
   CPPUNIT_TEST_SUITE_END();


public:

   void setUp()
   {
      // Create pool for data buffers
      mpPool = new MpBufPool(TEST_SAMPLES_PER_FRAME_SIZE * sizeof(MpAudioSample)
                             + MpArrayBuf::getHeaderSize(), BUFFER_NUM, "MpOutputManagerTest");
      CPPUNIT_ASSERT(mpPool != NULL);

      // Create pool for buffer headers
      mpHeadersPool = new MpBufPool(sizeof(MpAudioBuf), BUFFER_NUM, "MpOutputManagerTestHeaders");
      CPPUNIT_ASSERT(mpHeadersPool != NULL);

      // Set mpHeadersPool as default pool for audio and data pools.
      MpAudioBuf::smpDefaultPool = mpHeadersPool;
      MpDataBuf::smpDefaultPool = mpHeadersPool;
   }

   void tearDown()
   {
      if (mpPool != NULL)
      {
         delete mpPool;
      }
      if (mpHeadersPool != NULL)
      {
         delete mpHeadersPool;
      }
   }

   void testCreate()
   {
      for (int i=0; i<CREATE_TEST_RUNS_NUMBER; i++)
      {
         MpOutputDeviceManager manager(TEST_SAMPLES_PER_FRAME_SIZE,
                                       TEST_SAMPLES_PER_SECOND,
                                       TEST_MIXER_BUFFER_LENGTH);

      }
   }

   void testNonexistentDevice()
   {
      for (int i=0; i<NONEXISTENT_TEST_RUNS_NUMBER; i++)
      {
         MpOutputDeviceManager manager(TEST_SAMPLES_PER_FRAME_SIZE,
                                       TEST_SAMPLES_PER_SECOND,
                                       TEST_MIXER_BUFFER_LENGTH);

         CPPUNIT_ASSERT(manager.disableDevice(NONEXISTENT_DEVICE_ID) == OS_NOT_FOUND);
         CPPUNIT_ASSERT(manager.enableDevice(NONEXISTENT_DEVICE_ID) == OS_NOT_FOUND);

         MpOutputDeviceHandle deviceId;
         CPPUNIT_ASSERT(manager.getDeviceId(NONEXISTENT_DEVICE_NAME, deviceId) == OS_NOT_FOUND);

         UtlString deviceName;
         CPPUNIT_ASSERT(manager.getDeviceName(NONEXISTENT_DEVICE_ID, deviceName) == OS_NOT_FOUND);

         MpFrameTime bufferLength;
         CPPUNIT_ASSERT(manager.getMixerBufferLength(NONEXISTENT_DEVICE_ID, bufferLength) == OS_NOT_FOUND);
      }
   }

   void testAddRemoveToManager()
   {
      MpOutputDeviceHandle deviceId;

      OUTPUT_DRIVER device(OUTPUT_DRIVER_CONSTRUCTOR_PARAMS);
      CPPUNIT_ASSERT(!device.isEnabled());

      for (int i=0; i<ADD_REMOVE_TEST_RUNS_NUMBER; i++)
      {
         MpOutputDeviceManager deviceManager(TEST_SAMPLES_PER_FRAME_SIZE,
                                             TEST_SAMPLES_PER_SECOND,
                                             TEST_MIXER_BUFFER_LENGTH);

         deviceId = deviceManager.addDevice(&device);
         CPPUNIT_ASSERT(deviceId > 0);
         CPPUNIT_ASSERT(deviceManager.removeDevice(deviceId) == &device);
      }
   }

   void testEnableDisable()
   {
      MpOutputDeviceHandle deviceId;

      OUTPUT_DRIVER device(OUTPUT_DRIVER_CONSTRUCTOR_PARAMS);
      CPPUNIT_ASSERT(!device.isEnabled());

      for (int i=0; i<ENABLE_DISABLE_TEST_RUNS_NUMBER; i++)
      {
         // Test with mixer mode
         MpOutputDeviceManager deviceManager(TEST_SAMPLES_PER_FRAME_SIZE,
                                             TEST_SAMPLES_PER_SECOND,
                                             TEST_MIXER_BUFFER_LENGTH);

         deviceId = deviceManager.addDevice(&device);
         CPPUNIT_ASSERT(deviceId > 0);

         deviceManager.enableDevice(deviceId);
         CPPUNIT_ASSERT(deviceManager.isDeviceEnabled(deviceId));

         OsTask::delay(50);

         deviceManager.disableDevice(deviceId);
         CPPUNIT_ASSERT(!deviceManager.isDeviceEnabled(deviceId));

         CPPUNIT_ASSERT(deviceManager.removeDevice(deviceId) == &device);
      }
   }

   void testEnableDisableFast()
   {
      MpOutputDeviceHandle deviceId;

      OUTPUT_DRIVER device(OUTPUT_DRIVER_CONSTRUCTOR_PARAMS);
      CPPUNIT_ASSERT(!device.isEnabled());

      for (int i=0; i<ENABLE_DISABLE_TEST_RUNS_NUMBER; i++)
      {
         MpOutputDeviceManager deviceManager(TEST_SAMPLES_PER_FRAME_SIZE,
                                             TEST_SAMPLES_PER_SECOND,
                                             TEST_MIXER_BUFFER_LENGTH);

         deviceId = deviceManager.addDevice(&device);
         CPPUNIT_ASSERT(deviceId > 0);

         deviceManager.enableDevice(deviceId);
         CPPUNIT_ASSERT(deviceManager.isDeviceEnabled(deviceId));

         deviceManager.disableDevice(deviceId);
         CPPUNIT_ASSERT(!deviceManager.isDeviceEnabled(deviceId));

         CPPUNIT_ASSERT(deviceManager.removeDevice(deviceId) == &device);
      }
   }

   void testMixing()
   {
      MpOutputDeviceHandle deviceId;

      OUTPUT_DRIVER device(OUTPUT_DRIVER_CONSTRUCTOR_PARAMS);
      CPPUNIT_ASSERT(!device.isEnabled());

      for (int i=0; i<TICKER_TEST_WRITE_RUNS_NUMBER; i++)
      {
         MpOutputDeviceManager deviceManager(TEST_SAMPLES_PER_FRAME_SIZE,
                                             TEST_SAMPLES_PER_SECOND,
                                             TEST_MIXER_BUFFER_LENGTH);
         MpAudioBufPtr pBuffer = mpPool->getBuffer();

         deviceId = deviceManager.addDevice(&device);
         CPPUNIT_ASSERT(deviceId > 0);

         deviceManager.enableDevice(deviceId);
         CPPUNIT_ASSERT(deviceManager.isDeviceEnabled(deviceId));

         // Get current frame time and step to middle of mixer buffer to be in
         // time with output audio connection.
         MpFrameTime frameTime=deviceManager.getCurrentFrameTime(deviceId);
         frameTime += TEST_MIXER_BUFFER_LENGTH/2;

         // Write some data to device.
         for (int frame=0; frame<TEST_SAMPLE_FRAMES/TEST_SAMPLES_PER_FRAME_SIZE; frame++)
         {
            calculateSampleData(frameTime, pBuffer);

            OsTask::delay(TEST_FRAME_LENGTH_MS);
            CPPUNIT_ASSERT_EQUAL(OS_SUCCESS,
                                 deviceManager.pushFrame(deviceId, frameTime, pBuffer));
            frameTime += TEST_FRAME_LENGTH_MS;
         }

         deviceManager.disableDevice(deviceId);
         CPPUNIT_ASSERT(!deviceManager.isDeviceEnabled(deviceId));

         CPPUNIT_ASSERT(deviceManager.removeDevice(deviceId) == &device);
      }
   }

protected:
   MpBufPool *mpPool;         ///< Pool for data buffers
   MpBufPool *mpHeadersPool;  ///< Pool for buffers headers

};

CPPUNIT_TEST_SUITE_REGISTRATION(MpOutputDeviceManagerTest);
