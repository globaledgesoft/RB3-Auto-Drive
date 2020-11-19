# User Variables
set (SUPPORT_FUNCTION_CALL_TRACE 1)

# Common Include
include (${CAMX_PATH}/build/infrastructure/cmake/common.cmake)


# Files and Build Type
add_executable( hal3_test
     BufferManager.cpp
     QCameraHAL3Device.cpp
     QCameraHAL3Base.cpp
     QCameraHAL3TestSnapshot.cpp
     QCameraHALTestMain.cpp
     QCameraHAL3TestVideo.cpp
     QCameraTestVideoEncoder.cpp
     QCameraHAL3TestConfig.cpp
     QCameraHAL3TestPreview.cpp
     QCameraHAL3TestTOF.cpp
     TestLog.cpp
)

add_library( libomx_encoder SHARED
    OMX_Encoder.cpp
    BufferManager.cpp
)

add_library( libffbm_utils SHARED
     BufferManager.cpp
     QCameraHAL3Device.cpp
     QCameraHAL3Base.cpp
     QCameraHAL3TestSnapshot.cpp
     QCameraHAL3TestPreview.cpp
     QCameraHAL3TestTOF.cpp
     QCameraHAL3TestConfig.cpp
     TestLog.cpp
)

add_custom_command (
    OUTPUT  ${CMAKE_CURRENT_LIST_DIR}/g_version.h
    COMMAND perl ${CMAKE_CURRENT_LIST_DIR}/version.pl ${CMAKE_CURRENT_LIST_DIR}/g_version.h
    COMMENT "autogen version file"
)

add_custom_target (autogen_version_cameratest ALL
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/g_version.h
)



# Libraries to compile
target_link_libraries (hal3_test cutils)
target_link_libraries (hal3_test utils)
target_link_libraries (hal3_test log)
target_link_libraries (hal3_test hardware)
target_link_libraries (hal3_test camera_metadata)
target_link_libraries (hal3_test glib-2.0)
target_link_libraries (hal3_test dl)
target_link_libraries (hal3_test stdc++)
target_link_libraries (hal3_test atomic)
target_link_libraries (hal3_test pthread)
target_link_libraries (hal3_test libomx_encoder)
target_link_libraries (hal3_test camera_client)
target_link_libraries (hal3_test binder)

target_link_libraries (libomx_encoder cutils)
target_link_libraries (libomx_encoder utils)
target_link_libraries (libomx_encoder log)
target_link_libraries (libomx_encoder hardware)
target_link_libraries (libomx_encoder camera_metadata)
target_link_libraries (libomx_encoder glib-2.0)
target_link_libraries (libomx_encoder dl)
target_link_libraries (libomx_encoder stdc++)
target_link_libraries (libomx_encoder atomic)
target_link_libraries (libomx_encoder pthread)
target_link_libraries (libomx_encoder OmxCore)

target_link_libraries (libffbm_utils cutils)
target_link_libraries (libffbm_utils utils)
target_link_libraries (libffbm_utils log)
target_link_libraries (libffbm_utils hardware)
target_link_libraries (libffbm_utils camera_metadata)
target_link_libraries (libffbm_utils glib-2.0)
target_link_libraries (libffbm_utils dl)
target_link_libraries (libffbm_utils stdc++)
target_link_libraries (libffbm_utils atomic)
target_link_libraries (libffbm_utils pthread)
target_link_libraries (libffbm_utils camera_client)
target_link_libraries (libffbm_utils binder)


add_definitions (${CAMERA_TEST_CFLAGS})
add_definitions (${CAMERA_TEST_CPPFLAGS})
add_definitions( -DLINUX_ENABLED )

# Include Paths
include_directories (.)
include_directories (${KERNEL_INCDIR}/usr/include)
include_directories (${SYSTEM_INC_PATH}/system/core/include/cutils)
include_directories (${SYSTEM_INC_PATH}/system/core/include/system)
include_directories (${SYSTEM_INC_PATH}/system/core/libsystem/include)
include_directories (${SYSTEM_INC_PATH}/hardware/libhardware/include)
include_directories (${SYSTEM_INC_PATH}/hardware/qcom/media/mm-core/inc)
include_directories (${SYSTEM_INC_PATH}/hardware/qcom/display)
include_directories (${SYSTEM_INC_PATH}/qcom/display)
include_directories (${SYSTEM_INC_PATH}/hardware/libhardware/modules/gralloc)
include_directories (${SYSTEM_INC_PATH}/hardware/libhardware/include)
include_directories (${SYSTEM_INC_PATH}/media/hardware)
include_directories (${SYSTEM_INC_PATH}/libstagefrighthw)

add_definitions( -g )

if (CONFIG STREQUAL "Debug")
    add_definitions( -g )
endif()
if (LINUX_LE EQUAL True)
    add_definitions( -DLINUX_ENABLED )
endif()

add_definitions( -DUSE_GRALLOC1 )
add_definitions( -DDISABLE_META_MODE=1 )
add_definitions( -DCAMERA_STORAGE_DIR="/data/misc/camera/" )

#add_definitions( -E )

set (FASTMMI_INCLUDE_HEADERS
    QCameraHAL3TestConfig.h
    QCameraHAL3Test.h
    QCameraHAL3TestSnapshot.h
    QCameraHAL3TestPreview.h
    QCameraHAL3TestTOF.h
    QCameraHAL3Base.h
    BufferManager.h
    OMX_Encoder.h
    TestLog.h
    QCameraHAL3Device.h
    )

install(TARGETS libomx_encoder LIBRARY DESTINATION /usr/lib/ )
install(TARGETS libffbm_utils LIBRARY DESTINATION /usr/lib/ )
install(TARGETS hal3_test RUNTIME DESTINATION /usr/bin/ )
install(FILES ${FASTMMI_INCLUDE_HEADERS} DESTINATION include )
