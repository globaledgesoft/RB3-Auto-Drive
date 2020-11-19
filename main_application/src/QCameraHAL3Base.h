////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3Base.h
/// @brief Camera device Holder, Camera Test base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _QCAMERA_HAL3_BASE_
#define _QCAMERA_HAL3_BASE_

#include "QCameraHAL3Device.h"
class QCameraHAL3Base {
public:
    QCameraHAL3Base(camera_module_t* module,int CameraId);
    ~QCameraHAL3Base();
    QCameraHAL3Device* mDevice;
    int mCameraId;
protected:
    QCameraHAL3Base() = delete;
    camera_module_t* mModule;
};
#endif
