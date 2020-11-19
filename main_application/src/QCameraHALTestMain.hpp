#ifndef QCAM_MAIN
#define QCAM_MAIN

#define TOF_CAM_CMD "id=0,psize=640x480,pformat=raw16,dsize=640x480,dformat=raw16"
#define P_CAM_CMD "id=1,psize=1920x1080,pformat=yuv420"
#define MAX_CAMERA 4
#define DEPTH_CAM_ID 0
#define MAIN_CAM_ID 1
#define SNPE_MODEL "SNPE"
#define NEODLR_MODEL "NEODLR"
#define SNPE_CLASS_MODEL "/home/rb3-auto-drive/artifacts/snpe_models/classification-model-2499.dlc"
#define SNPE_SEG_MODEL "/home/rb3-auto-drive/artifacts/snpe_models/seg-model-15199.dlc"
#define BOT_STOP_CMD "python /bot_init.py"
#define BOT_FORWARD_CMD "python /bot_forward.py"
#define BOT_BYPASS_CMD "python /bot_bypass.py"
#define DIST_TH_FAR 25
#define DIST_TH_NEAR 2
#define TOF_DEPTH_H 480
#define TOF_DEPTH_W 640
#define DLR_SEG_H 256
#define DLR_SEG_W 256
#define DLR_SEG_C 3
#define IMG_ROWS 1080
#define IMG_COLS 1920
#define SNPE_SEG_W 320
#define SNPE_SEG_H 240
#define SNPE_SEG_C 3
#define SNPE_CLASS_W 128
#define SNPE_CLASS_H 128
#define SNPE_CLASS_C 3
#endif
