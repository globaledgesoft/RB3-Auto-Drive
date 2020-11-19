##### Make sure all is the first target.
all:

CXX ?= g++
CC  ?= gcc

CFLAGS += -I./src/ -DLINUX_ENABLED  -DUSE_GRALLOC1  -DDISABLE_META_MODE=1 
CFLAGS += -DCAMERA_STORAGE_DIR="\"/home/preview/\"" -I./include -I $(SNPE_ROOT)/include/zdl
CFLAGS += -g -pthread -Wall 
CFLAGS += `pkg-config --cflags --libs opencv`

LDFLAGS += -lcutils -lutils -llog
LDFLAGS += -lSNPE
LDFLAGS += -lSNPESample
LDFLAGS += -lhardware
LDFLAGS += -lcamera_metadata
LDFLAGS += -lglib-2.0
LDFLAGS += -ldl
LDFLAGS += -lstdc++
LDFLAGS += -latomic
LDFLAGS += -lpthread
#LDFLAGS += -lomx_encoder
LDFLAGS += -lOmxCore
LDFLAGS += -lcamera_client
LDFLAGS += -lbinder

C_SRC=
CXX_SRC=
OBJ=
DEP=

CXX_SRC+=src/BufferManager.cpp
CXX_SRC+=src/QCameraHAL3Device.cpp
CXX_SRC+=src/QCameraHAL3Base.cpp
CXX_SRC+=src/QCameraHAL3TestSnapshot.cpp
CXX_SRC+=src/client.cpp
CXX_SRC+=src/QCameraHALTestMain.cpp
CXX_SRC+=src/QCameraHAL3TestVideo.cpp
CXX_SRC+=src/QCameraTestVideoEncoder.cpp
CXX_SRC+=src/QCameraHAL3TestConfig.cpp
CXX_SRC+=src/QCameraHAL3TestPreview.cpp
CXX_SRC+=src/QCameraHAL3TestTOF.cpp
CXX_SRC+=src/TestLog.cpp
CXX_SRC+=src/OMX_Encoder.cpp
CXX_SRC+=src/BufferManager.cpp
CXX_SRC+=src/BBox.cpp
CXX_SRC+=src/DepthCalc.cpp

OBJ_CE = src/QCameraHALTestMain.o

rb3_autodrive: $(OBJ_CE)
TARGET_OBJ += $(OBJ_CE)
TARGETS    += rb3_autodrive

include ./common.mk
