#ifndef PREDICT_H
#define PREDICT_H

#include <string>
#include <unordered_map>
#include <vector>

#include "SNPE/SNPE.hpp"
#include "DlSystem/ITensor.hpp"
#include "DlSystem/UserBufferMap.hpp"

std::vector<uint8_t> predictSnpe(std::unique_ptr<zdl::SNPE::SNPE> &snpe1, std::unique_ptr<zdl::SNPE::SNPE> &snpe2, std::vector<uint8_t> &ipClsBuffer, std::vector<uint8_t> &ipSegBuffer);

#endif