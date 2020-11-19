
#undef LOCAL_TAG
#define LOCAL_TAG "MAIN"
#define LOG_TAG "CameraTest"

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include "inttypes.h"
#include <iostream>
#include <fstream>
#include <log/log.h>
#include "QCameraHAL3TestSnapshot.h"
#include "QCameraHAL3TestVideo.h"
#include "QCameraHAL3TestPreview.h"
#include "QCameraHAL3TestTOF.h"
#include <string.h>
#include <camera/VendorTagDescriptor.h>
#include <mutex>
#include "g_version.h"
#include "QCameraHAL3Base.h"
#include <signal.h>
#include "TestLog.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QCameraHALTestMain.hpp"
#include "LoadModel.hpp"
#include "Predict.hpp"
#include "BBox.hpp"
#include "DepthCalc.hpp"
#include "client.hpp"

// Declaration of thread condition variable 
pthread_cond_t cond_f = PTHREAD_COND_INITIALIZER; 
  
// declaring mutex 
pthread_mutex_t lock_f = PTHREAD_MUTEX_INITIALIZER; 

using namespace cv;
using namespace std;
using namespace android;

extern char *optarg;
bool IsCommandFile = false;
ifstream FileStream;

camera_module_t *CamModule;
QCameraHAL3Base *Camera[MAX_CAMERA];
QCameraHAL3Test *HAL3Test[MAX_CAMERA];
int CurCameraId;

string cmd_order[2] = {
    TOF_CAM_CMD,
    P_CAM_CMD };


/************************************************************************
* name : CameraDeviceStatusChange
* function: static callback forwarding methods from HAL to instance
************************************************************************/
void CameraDeviceStatusChange(
    const struct camera_module_callbacks *callbacks,
    int camera_id,
    int new_status)
{
}

void TorchModeStatusChange(
    const struct camera_module_callbacks *callbacks,
    const char *camera_id,
    int new_status)
{
}
camera_module_callbacks_t module_callbacks = {CameraDeviceStatusChange,
                                              TorchModeStatusChange};
#ifdef LINUX_ENABLED
#define CAMERA_HAL_LIBERAY "/usr/lib/hw/camera.qcom.so"
#else
#define CAMERA_HAL_LIBERAY "/usr/lib/hw/camera.qcom.so"
#endif

/************************************************************************
* name : loadCameraModule
* function: load the camera module liberay
************************************************************************/
static int loadCameraModule(const char *id, const char *path,
                            camera_module_t **pCmi)
{
    int status = -EINVAL;
    void *handle = NULL;
    camera_module_t *cmi = NULL;

    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL)
    {
        char const *err_str = dlerror();
        ALOGE("load: module=%s\n%s", path, err_str ? err_str : "unknown");
        status = -EINVAL;
        cmi = NULL;
        *pCmi = cmi;
        return status;
    }

    /* Get the address of the struct hal_module_info. */
    const char *sym = HAL_MODULE_INFO_SYM_AS_STR;
    cmi = (camera_module_t *)dlsym(handle, sym);
    if (cmi == NULL)
    {
        ALOGE("load: couldn't find symbol %s", sym);
        status = -EINVAL;
        if (handle != NULL)
        {
            dlclose(handle);
            handle = NULL;
        }
        *pCmi = cmi;
        return status;
    }

    /* Check that the id matches */
    if (strcmp(id, cmi->common.id) != 0)
    {
        ALOGE("load: id=%s != cmi->id=%s", id, cmi->common.id);
        status = -EINVAL;
        if (handle != NULL)
        {
            dlclose(handle);
            handle = NULL;
        }
        *pCmi = cmi;
        return status;
    }

    cmi->common.dso = handle;
    /* success */
    status = 0;
    ALOGI("loaded HAL id=%s path=%s cmi=%p handle=%p",
          id, path, *pCmi, handle);
    *pCmi = cmi;

    return status;
}

/************************************************************************
* name : initialize
* function: initialize the camera module for camera test
************************************************************************/
int initialize()
{

    struct camera_info info;
    vendor_tag_ops_t vendor_tag_ops_;
    //Load camera module
    int err = loadCameraModule(CAMERA_HARDWARE_MODULE_ID, CAMERA_HAL_LIBERAY, &CamModule);
    //init module
    err = CamModule->init();
    // get Vendor Tag
    if (CamModule->get_vendor_tag_ops)
    {
        vendor_tag_ops_ = vendor_tag_ops_t();
        CamModule->get_vendor_tag_ops(&vendor_tag_ops_);

        sp<VendorTagDescriptor> vendor_tag_desc;
        err = VendorTagDescriptor::createDescriptorFromOps(&vendor_tag_ops_,
                                                           vendor_tag_desc);

        if (0 != err)
        {
            ALOGE("%s: Could not generate descriptor from vendor tag operations,"
                  "received error %s (%d). Camera clients will not be able to use"
                  "vendor tags",
                  __FUNCTION__, strerror(err), err);
            return err;
        }

        // Set the global descriptor to use with camera metadata
        err = VendorTagDescriptor::setAsGlobalVendorTagDescriptor(vendor_tag_desc);

        if (0 != err)
        {
            ALOGE("%s: Could not set vendor tag descriptor, "
                  "received error %s (%d). \n",
                  __func__, strerror(-err), err);
            return err;
        }
    }

    //set callback
    err = CamModule->set_callbacks(&module_callbacks);
    //Get camera info and show to user
    int nuCameras = CamModule->get_number_of_cameras();
    for (int i = 0; i < nuCameras; i++)
    {
        auto rc = CamModule->get_camera_info(i, &info);
        if (!rc)
        {
            printf("Camera: %d face:%d\n", i, info.facing);
        }
        else
        {
            printf("Error Get Camera:%d Info \n", i);
            return rc;
        }
    }

    return 0;
}

static bool stop = false;
static void deleteCameraInstance(int signum)
{
    ALOGI("========we receive signal %d", signum);

    if (signum == SIGIO ||
        signum == SIGHUP ||
        signum == SIGINT ||
        signum == SIGQUIT ||
        signum == SIGILL ||
        signum == SIGABRT ||
        signum == SIGFPE ||
        signum == SIGINT ||
        signum == SIGSEGV ||
        signum == SIGALRM ||
        signum == SIGTERM)
    {
        ALOGI("========we receive signal %d and will exit", signum);
    }
    else
    {
        ALOGI("========we receive signal %d return it !", signum);
        return;
    }

    for (int i = 0; i < MAX_CAMERA; i++)
    {
        if (!HAL3Test[i])
        {
            continue;
        }
        if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_SNAPSHOT)
        {
            QCameraHAL3TestSnapshot *testSnapshot =
                (QCameraHAL3TestSnapshot *)HAL3Test[i];
            testSnapshot->stop();
            if (testSnapshot->mConfig)
                delete testSnapshot->mConfig;
            testSnapshot->mConfig = NULL;
            delete testSnapshot;
        }
        else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_VIDEO)
        {
            QCameraHAL3TestVideo *testVideo =
                (QCameraHAL3TestVideo *)HAL3Test[i];
            testVideo->stop();
            if (testVideo->mConfig)
                delete testVideo->mConfig;
            testVideo->mConfig = NULL;
            delete testVideo;
        }
        else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_PREVIEW)
        {
            QCameraHAL3TestPreview *testPreview =
                (QCameraHAL3TestPreview *)HAL3Test[i];
            testPreview->stop();
            if (testPreview->mConfig)
                delete testPreview->mConfig;
            testPreview->mConfig = NULL;
            delete testPreview;
        }
        else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_TOF)
        {
            QCameraHAL3TestTOF *testTof =
                (QCameraHAL3TestTOF *)HAL3Test[i];
            testTof->stop();
            if (testTof->mConfig)
                delete testTof->mConfig;
            testTof->mConfig = NULL;
            delete testTof;
        }
        HAL3Test[i] = NULL;

        if (Camera[i])
        {
            Camera[i]->mDevice->closeCamera();
            delete Camera[i];
            Camera[i] = NULL;
        }
    }

    ALOGI("========we receive signal %d finish process it !", signum);
    stop = true;
    exit(0);

    return;
}

/************************************************************************
* name : channelFirst
* function: function to convert image in channel first ordering
************************************************************************/
float *channelFirst(float *data) 
{
    int c = DLR_SEG_C;
    int w = DLR_SEG_W;
    int h = DLR_SEG_H;
    int fold_w_h = DLR_SEG_W*DLR_SEG_H;
    int fold_c_w = c*DLR_SEG_W;
    int fold_c_img_w = c*DLR_SEG_W;

    float * data_cp = (float *)malloc(DLR_SEG_H*DLR_SEG_W*DLR_SEG_C*sizeof(float));
    for(int k = 0; k < c; k++) {
        for(int j = 0; j < h; j++) {
            for(int i = 0; i < w; i++) {
                  
                    int dst_f = i + w*j + fold_w_h*k;
               
                    int dst_d = k + c*i + fold_c_w*j;
		    data_cp[dst_f] = data[dst_d];
	    }
	}
    }
    return data_cp;
}

int main(int argc, char *argv[])
{
    int res = 0;
    int modelChoice = 0;
    int sockfd;
    ssize_t size = 0;
    static std::string dlc1 = SNPE_CLASS_MODEL;
    static std::string dlc2 = SNPE_SEG_MODEL;
    uint16_t *depthImage;
    Mat image;
    Mat RGBImage;
    Mat imgClass;
    Mat imgSeg;
    Mat depth;
    Mat modelOut;
    Mat dlrInput;
    Mat segImage;
    Mat resizedSegImage;
    std::vector<float> dlrOut (DLR_SEG_H * DLR_SEG_W, 0.0);
    int x = IMG_COLS;
    int y = (int)(IMG_ROWS * 1.5);
    int framesize = x * y;
    int num = 1;
    int dist = 0;
    int N;
    int cpIndex;

    if (argc < 2 || argc > 2) {
        printf("Please mention the command line parameters properly.\nUsage: rb3_autodrive MODEL_NAME\nModel Names: [SNPE, NEODLR]\n");
        exit(0);
    }
    if (!strcmp(argv[1], SNPE_MODEL)) {
        modelChoice = 1;
    } else if (!strcmp(argv[1], NEODLR_MODEL)) {
        modelChoice = 0;
    } else {
        printf("Wrong Model Name %s\nPlease mention the command line parameters properly.\nUsage: rb3_autodrive MODEL_NAME\nModel Names: [SNPE, NEODLR]\n", argv[1]);
        exit(0);
    }
    for (int i = 0; i < MAX_CAMERA; i++) {
        Camera[i] = NULL;
    }
    for (int sig = 1; sig <= SIGRTMAX; sig++) {
        signal(sig, deleteCameraInstance);
    }
    printf("Initializing\n");
    res = initialize();
    if (res != 0) {
        printf("Failed\n");
        return -1;
    }
    printf("Adding Camera\n");
    for (int str_indx = 0; str_indx < 2; str_indx++) {
        TestConfig *testConf = new TestConfig();
        res = testConf->parseCommandlineAdd(size, (char *)cmd_order[str_indx].c_str());
        if (res != 0) {
            printf("error command order res:%d\n", res);
            break;
        }
        CurCameraId = testConf->mCameraId;
        printf("Added a camera :%d\n", CurCameraId);
        printf("test mode:%d[TESTMODE_TOF:%d]\n", testConf->mTestMode, TESTMODE_TOF);
        if (Camera[str_indx] == NULL) {
            Camera[CurCameraId] = new QCameraHAL3Base(CamModule, CurCameraId);
        }
        switch (testConf->mTestMode) {
            case TESTMODE_PREVIEW: {
                QCameraHAL3TestPreview *testPreview = new QCameraHAL3TestPreview(CamModule,
                                                                                Camera[CurCameraId]->mDevice, testConf);
                testPreview->PreinitPreviewStream();
                testPreview->mDevice->openCamera();
                testPreview->run();
                testPreview->rgb_copy = 0;
                HAL3Test[testConf->mCameraId] = testPreview;
                break;
            }
            case TESTMODE_TOF: {
                QCameraHAL3TestTOF *testTof = new QCameraHAL3TestTOF(CamModule,
                                                                    Camera[CurCameraId]->mDevice, testConf);
                testTof->PreinitTofStreams();
                testTof->mDevice->openCamera();
                /*tof new*/
                testTof->TofLoadEeprom();
                testTof->TofInitStruct();
                testTof->TofAfeCalculate();
                testTof->TofCalExp();
                testTof->TofTransmit();
                testTof->depth_copy = 0;
                /*tof end*/
                testTof->run();
                HAL3Test[testConf->mCameraId] = testTof;
                break;
            }
            case TESTMODE_SNAPSHOT: {
                //test snapshot
                QCameraHAL3TestSnapshot *testSnapshot = new QCameraHAL3TestSnapshot(CamModule,
                                                                                    Camera[CurCameraId]->mDevice, testConf);
                testSnapshot->PreinitSnapshotStreams();
                testSnapshot->mDevice->openCamera();
                testSnapshot->run();
                HAL3Test[testConf->mCameraId] = testSnapshot;
                break;
            }
            case TESTMODE_VIDEO: {
                QCameraHAL3TestVideo *testVideo = new QCameraHAL3TestVideo(CamModule,
                                                                        Camera[CurCameraId]->mDevice, testConf);
                testVideo->PreinitVideoStreams();
                testVideo->mDevice->openCamera();
                testVideo->run();
                HAL3Test[testConf->mCameraId] = testVideo;
                break;
            }
            default: {
                printf("Wrong TEST MODE\n");
                break;
            }
        }
    }
    QCameraHAL3TestPreview *testPreview = (QCameraHAL3TestPreview *)HAL3Test[1];
    if (modelChoice) {
        /* Model Loading for SNPE */
        testPreview->snpe1 = loadModel(dlc1);
        testPreview->snpe2 = loadModel(dlc2);
    } else {
        /* Load Sagemaker Model */
        sockfd = getFd();
    }
    for (int j = 0; j < 10; j++) {
        cout<<"Running LOOP No: "<<j<<endl;
            if (HAL3Test[MAIN_CAM_ID]->mConfig->mTestMode == TESTMODE_PREVIEW) {
		        /* Preview Call & SNPE Processing */
                pthread_mutex_lock(&lock_f); 
                QCameraHAL3TestPreview *testPreview = (QCameraHAL3TestPreview *)HAL3Test[MAIN_CAM_ID];
                testPreview->dumpPreview(num);
                QCameraHAL3TestTOF *testTof = (QCameraHAL3TestTOF *)HAL3Test[DEPTH_CAM_ID];
		        testTof->dumpPreview(num);
                pthread_cond_wait(&cond_f, &lock_f);
            	image.create(y, x, CV_8UC1);
            	memcpy(image.data, testPreview->image_data, framesize);
                free(testPreview->image_data);
            	cvtColor(image, RGBImage, COLOR_YUV2RGB_NV21);
                flip(RGBImage,RGBImage,0);
                flip(RGBImage,RGBImage,1);
                if (modelChoice) {
                    /* Inference for SNPE */
                    resize(RGBImage, imgSeg, Size(SNPE_SEG_W, SNPE_SEG_H));
                    imgSeg.convertTo(imgSeg,CV_32FC3, 1.0/255.0);
                    resize(RGBImage, imgClass, Size(SNPE_CLASS_W, SNPE_CLASS_H));
                    imgClass.convertTo(imgClass,CV_32FC3, 1.0/255.0);
                    modelOut.create(SNPE_SEG_H, SNPE_SEG_W, CV_32FC1);
                    N = SNPE_CLASS_W * SNPE_CLASS_H * SNPE_CLASS_C * sizeof(float);
                    std::vector<uint8_t> ipClsBuffer(imgClass.data, imgClass.data+N);
                    N = SNPE_SEG_H * SNPE_SEG_W * SNPE_SEG_C * sizeof(float);
                    std::vector<uint8_t> ipSegBuffer(imgSeg.data, imgSeg.data+N);
                    std::vector<uint8_t> snpeOut;
                    snpeOut = predictSnpe(testPreview->snpe1, testPreview->snpe2, ipClsBuffer, ipSegBuffer);
                    if (snpeOut.empty()) {
                        std::cout<<"No object"<<std::endl;
                        system(BOT_FORWARD_CMD);
			            testTof->depth_copy = 0;
			            testPreview->rgb_copy = 0;
		                free(testTof->depth_data);
			            pthread_mutex_unlock(&lock_f);
			            continue;
                    } else {
                        cpIndex = 0;
                        for(unsigned int i = 0; i < snpeOut.size(); i+=4) {
                            for(int j=0; j<4; j++) {
                                modelOut.data[cpIndex] = snpeOut.at(i);
                                i++;
                                cpIndex++;
                            }
                        }
                        modelOut.convertTo(modelOut,CV_8UC1, 255);
                        modelOut.copyTo(segImage);
                    }
                } else {
                    /* Inference for NEO DLR */
                    float *data = NULL;
                    modelOut.create(DLR_SEG_H, DLR_SEG_W,CV_32FC1);
                    N = DLR_SEG_H * DLR_SEG_W * DLR_SEG_C;
                    resize(RGBImage, dlrInput, Size(DLR_SEG_W,DLR_SEG_H));
                    dlrInput.convertTo(dlrInput,CV_32FC3,1.0/255.0);
                    data = (float *)dlrInput.data;
		            data = channelFirst(data);
                    std::vector<float> dlrInputVec(data, data+N);
                    dlrOut = predictDlr(sockfd, dlrInputVec);
		            free(data);
                    if(dlrOut[0] == -1.0) {
                        std::cout<<"No object"<<std::endl;
                        system(BOT_FORWARD_CMD);
			            testTof->depth_copy = 0;
			            testPreview->rgb_copy = 0;
		                free(testTof->depth_data);
			            pthread_mutex_unlock(&lock_f);
			            continue;
                    } else {
                        std::cout<<"object present"<<std::endl;
                        modelOut.data = (uint8_t *)dlrOut.data();
                        modelOut.convertTo(modelOut,CV_8UC1, 255);
                        resize(modelOut, resizedSegImage, Size(320, 240));
                        resizedSegImage.copyTo(segImage);
                    }
                }
                depthImage = testTof->depth_data;   
                if(depthImage == NULL) {
                    std::cout<<"No Depth data Found"<<std::endl;
                    pthread_mutex_unlock(&lock_f);
                    continue;
                } 
                depth.create(TOF_DEPTH_H, TOF_DEPTH_W, CV_16UC1);
                memcpy(depth.data, testTof->depth_data, ((TOF_DEPTH_H*TOF_DEPTH_W)*2));
                depth.convertTo(depth,CV_32FC1, 1.0/4096.0);
                depth.convertTo(depth,CV_8UC1, 255.0);
                std::list<Rectangle*> rect = getBbox(segImage);
                if (rect.empty()) {
                    std::cout<<"No bounding box detected"<<std::endl;
                    testTof->depth_copy = 0;
                    testPreview->rgb_copy = 0;
                    free(testTof->depth_data);
                    system(BOT_STOP_CMD);
                    pthread_mutex_unlock(&lock_f);
                    continue;
                }
                dist = getDepth(depthImage, rect);
                dist = (int)(dist / 10.0);
                std::cout<<"Distance: "<<dist<<std::endl;
                testTof->depth_copy = 0;
                testPreview->rgb_copy = 0;
                free(testTof->depth_data);
                if(j == 0 || dist == 0) {
                    std::cout<<"Stop"<<std::endl;
                    system(BOT_STOP_CMD);
                } else if(dist>DIST_TH_FAR) {
                    std::cout<<"Forward"<<std::endl;
                    system(BOT_FORWARD_CMD);
                } else if(dist>DIST_TH_NEAR) {
                    std::cout<<"Bypass"<<std::endl;
                    system(BOT_BYPASS_CMD);
                }
                pthread_mutex_unlock(&lock_f); 
            }
    }
    std::cout<<"Quiting..."<<std::endl;
    for (int i = 0; i < MAX_CAMERA; i++) {
        if (!HAL3Test[i]) {
            continue;
        }
        if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
            QCameraHAL3TestSnapshot *testSnapshot =
                (QCameraHAL3TestSnapshot *)HAL3Test[i];
            testSnapshot->stop();
            delete testSnapshot->mConfig;
            testSnapshot->mConfig = NULL;
            delete testSnapshot;
        }
        else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_VIDEO) {
            QCameraHAL3TestVideo *testVideo =
                (QCameraHAL3TestVideo *)HAL3Test[i];
            testVideo->stop();
            delete testVideo->mConfig;
            testVideo->mConfig = NULL;
            delete testVideo;
        }
        else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_PREVIEW) {
            QCameraHAL3TestPreview *testPreview =
                (QCameraHAL3TestPreview *)HAL3Test[i];
            testPreview->stop();
            delete testPreview->mConfig;
            testPreview->mConfig = NULL;
            delete testPreview;
        }
        else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_TOF) {
            QCameraHAL3TestTOF *testTof =
                (QCameraHAL3TestTOF *)HAL3Test[i];
            testTof->stop();
            delete testTof->mConfig;
            testTof->mConfig = NULL;
            delete testTof;
        }
        HAL3Test[i] = NULL;
    }
    stop = true;
    for (int i = 0; i < MAX_CAMERA; i++) {
        if (Camera[i]) {
            Camera[i]->mDevice->closeCamera();
            delete Camera[i];
            Camera[i] = NULL;
        }
    }
    close(sockfd);
    std::cout<<"Exited Application"<<std::endl;
    return 0;
}
