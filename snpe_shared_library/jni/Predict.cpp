#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include "CheckRuntime.hpp"
#include "LoadContainer.hpp"
#include "SetBuilderOptions.hpp"
#include "LoadInputTensor.hpp"
#include "udlExample.hpp"
#include "CreateUserBuffer.hpp"
#include "PreprocessInput.hpp"
#include "SaveOutputTensor.hpp"
#include "Util.hpp"
#include "DlSystem/DlError.hpp"
#include "DlSystem/RuntimeList.hpp"
#include "DlSystem/UserBufferMap.hpp"
#include "DlSystem/UDLFunc.hpp"
#include "DlSystem/IUserBuffer.hpp"
#include "DlContainer/IDlContainer.hpp"
#include "SNPE/SNPE.hpp"
#include "DiagLog/IDiagLog.hpp"

/************************************************************************
* name : predictSnpe
* function: Predict output for classification model and segmentation model
************************************************************************/
std::vector<uint8_t> predictSnpe(std::unique_ptr<zdl::SNPE::SNPE> &snpe1, std::unique_ptr<zdl::SNPE::SNPE> &snpe2,std::vector<uint8_t> &ipClsBuffer, std::vector<uint8_t> &ipSegBuffer)
{
    std::vector<uint8_t> classBuffer;
    std::vector<uint8_t> segBuffer;
    std::vector<uint8_t> emptyBuffer;
    unsigned char cls_buf[4];
    bool execStatus = false;
    std::string outputDir1, outputDir2;
    zdl::DlSystem::UserBufferMap inputMap1, outputMap1, inputMap2, outputMap2;
    std::vector <std::unique_ptr<zdl::DlSystem::IUserBuffer>> snpeUserBackedInputBuffers1, snpeUserBackedOutputBuffers1, snpeUserBackedInputBuffers2, snpeUserBackedOutputBuffers2;
    std::unordered_map <std::string, std::vector<uint8_t>> applicationOutputBuffers1, applicationOutputBuffers2;
    std::unordered_map <std::string, std::vector<uint8_t>> applicationInputBuffers1;
    
    createOutputBufferMap(outputMap1, applicationOutputBuffers1, snpeUserBackedOutputBuffers1, snpe1, false);
    createInputBufferMap(inputMap1, applicationInputBuffers1, snpeUserBackedInputBuffers1, snpe1, false);
    
    if(!loadInputUserBufferFloatFromBufer(applicationInputBuffers1, snpe1, ipClsBuffer))
    {
        return emptyBuffer;
    }

    // Execute the input buffer map on the model with SNPE
    execStatus = snpe1->execute(inputMap1, outputMap1);
    // Save the execution results only if successful
    if (execStatus == true)
    {
        if(!saveOutput(outputMap1, applicationOutputBuffers1, classBuffer))
        {
            return emptyBuffer;
        }
    }
    else
    {
        std::cerr << "Error while executing the network." << std::endl;
        return emptyBuffer;
    }

    for (int i=0; i<classBuffer.size(); i++) {
	    cls_buf[i] = classBuffer.at(i);
    }
    float *data = (float *)cls_buf;

    // Segmentation Model
    std::cout<<"Output of Classification Model: "<<*data<<std::endl;
    if((int)*data != 4) 
    {
        createOutputBufferMap(outputMap2, applicationOutputBuffers2, snpeUserBackedOutputBuffers2, snpe2, false);
        std::unordered_map <std::string, std::vector<uint8_t>> applicationInputBuffers2;
        createInputBufferMap(inputMap2, applicationInputBuffers2, snpeUserBackedInputBuffers2, snpe2, false);

        if(!loadInputUserBufferFloatFromBufer(applicationInputBuffers2, snpe2, ipSegBuffer))
        {
            return emptyBuffer;
        }

        // Execute the input buffer map on the model with SNPE
        execStatus = snpe2->execute(inputMap2, outputMap2);
        // Save the execution results only if successful
        if (execStatus == true)
        {
            if(!saveOutput(outputMap2, applicationOutputBuffers2, segBuffer))
            {
                return emptyBuffer;
            }
            return segBuffer;
        }
        else
        {
            std::cerr << "Error while executing the network." << std::endl;
            return emptyBuffer;
        }
    }
    else 
    {
        std::cout<< "No object in frame" << std::endl;
        return emptyBuffer;
    }
    return emptyBuffer;
}
