#ifndef CLIENT_H
#define CLIENT_H

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
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "QCameraHALTestMain.hpp"

#define PORT "3490"
#define LOOPBACK_ADDR "127.0.0.1"
#define NO_BYTES 1024*4

void *getInAddr(struct sockaddr *sa);
int getFd();
std::vector<float> predictDlr(int sockfd, std::vector<float> ipBuf);

#endif
