////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3Test.h
/// @brief base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_
#define _QCAMERA_HAL3_TEST_
#include "QCameraHAL3Device.h"
#include "QCameraHAL3TestConfig.h"
#define JPEG_QUALITY_DEFAULT 85
#define PREVIEW_STREAM_BUFFER_MAX 12
#define DEPTH_STREAM_BUFFER_MAX 12
#define SNAPSHOT_STREAM_BUFFER_MAX 8
#define VIDEO_STREAM_BUFFER_MAX 18

typedef enum {
    PREVIEW_TYPE,
    SNAPSHOT_TYPE,
    VIDEO_TYPE,
}StreamType;
typedef struct _StreamCapture{
    StreamType type;
    int count;
}StreamCapture;
class QCameraHAL3Test {
public:
    QCameraHAL3Device* mDevice;
    TestConfig* mConfig;
protected:
    camera_module_t* mModule;
};
#endif
