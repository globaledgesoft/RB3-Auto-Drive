////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestTOF.h
/// @brief test TOF case
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_RAW_
#define _QCAMERA_HAL3_TEST_RAW_
#include "QCameraHAL3Test.h"
#include <utils/Timers.h>
#include "tl_dev_eeprom.h"

class QCameraHAL3TestTOF :public DeviceCallback, public QCameraHAL3Test{
public:
    QCameraHAL3TestTOF(camera_module_t* module, QCameraHAL3Device* device, TestConfig* config);
    ~QCameraHAL3TestTOF();
    void run();
    void stop();
    void dumpPreview(int count);
    uint16_t *depth_data;
    int depth_copy;
    void CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result) override;
    void HandleMetaData(DeviceCallback* cb, camera3_capture_result *result) override;
    void updataMetaDatas(android::CameraMetadata* meta);
    android::CameraMetadata* getCurrentMeta();
    void setCurrentMeta(android::CameraMetadata* meta);
    typedef void (*ffbmRawCb)(void*, int);
    void setFfbmRawCb(ffbmRawCb);

    ffbmRawCb mFfbmDepthCb;
    int PreinitTofStreams();
    /*tof add calculate*/
    int TofLoadEeprom();
    void TofInitStruct();
    int TofAfeCalculate();
    int TofCalExp();
    int TofTransmit();
    /*end*/
    int                      mDumpPreviewNum;
private:
    unsigned int                      mFrameCount;
    int                      mLastFrameCount;
    nsecs_t                  mLastFpsTime;
    android::CameraMetadata* mMetadataExt;
    /*tof add*/
    tl_temp_val temp_val;
    tl_dev_eeprom dev_eeprom;
    uint16_t exp_val;
    /*tof end*/

    int initTofStreams();
    void showPreviewFPS();

};
#endif
