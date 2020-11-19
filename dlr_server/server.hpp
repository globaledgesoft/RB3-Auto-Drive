#ifndef SERVER_H
#define SERVER_H
#include "dlr.h"
#include <libgen.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"
#define BACKLOG 10
#define NO_BYTES 1024*4
#define CLASS_IP_NODE "input/image_input"
#define SEG_IP_NODE "input"
#define CLASS_MODEL_PATH "/rb3-auto-drive/artifacts/dlr_models/clsModel"
#define SEG_MODEL_PATH "/rb3-auto-drive/artifacts/dlr_models/segModel"
#define DLR_SEG_H 256
#define DLR_SEG_W 256
#define DLR_SEG_C 3
#define DLR_CLASS_H 128
#define DLR_CLASS_W 128
#define DLR_CLASS_C 3

template <typename T>
void argmax(int& argmax, int& max_id, T& max_pred);

template <typename T>
void RunInference(DLRModelHandle model, std::vector<T> in_data,
                  const std::string& input_name,
                  std::vector<std::vector<T>>& outputs, const std::string& model_name);

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);

#endif
