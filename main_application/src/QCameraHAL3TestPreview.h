////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestPreview.h
/// @brief preview only mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_PREVIEW_
#define _QCAMERA_HAL3_TEST_PREVIEW_
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include "DlSystem/DlError.hpp"
#include "DlSystem/RuntimeList.hpp"
#include "DlSystem/UserBufferMap.hpp"
#include "DlSystem/UDLFunc.hpp"
#include "DlSystem/IUserBuffer.hpp"
#include "DlContainer/IDlContainer.hpp"
#include "SNPE/SNPE.hpp"
#include "DiagLog/IDiagLog.hpp"
#include "QCameraHAL3Test.h"
#include <utils/Timers.h>

class QCameraHAL3TestPreview :public DeviceCallback, public QCameraHAL3Test{
public:
    QCameraHAL3TestPreview(camera_module_t* module,QCameraHAL3Device* device, TestConfig* config);
    ~QCameraHAL3TestPreview();
    void run();
    void stop();
    void dumpPreview(int count);
    void CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result) override;
    void HandleMetaData(DeviceCallback* cb, camera3_capture_result *result) override;
    void updataMetaDatas(android::CameraMetadata* meta);
    std::unique_ptr<zdl::SNPE::SNPE> snpe1, snpe2;
    unsigned char *image_data;
    int rgb_copy;
    android::CameraMetadata* getCurrentMeta();
    void setCurrentMeta(android::CameraMetadata* meta);
    typedef void (*ffbmPreviewCb)(void*, int);
    void setFfbmPreviewCb(ffbmPreviewCb);
    ffbmPreviewCb mFfbmPreviewCb;

    int mDumpPreviewNum;
    int PreinitPreviewStream();
private:
    unsigned int mFrameCount;
    int mLastFrameCount;
    nsecs_t mLastFpsTime;
    android::CameraMetadata* mMetadataExt;

    int initPreviewStream();
    void showPreviewFPS();
};
#endif
