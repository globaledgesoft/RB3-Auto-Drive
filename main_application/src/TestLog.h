////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  TestLog.h
/// @brief testlog class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TESTLOG_
#define _TESTLOG_
#include <string>
using namespace std;

#define TEST_INFO(format, x...) \
	ALOGE("%s(%d) " format, __func__, __LINE__, ##x)

#define TEST_DEG(format, x...) \
	ALOGD("%s(%d) " format, __func__, __LINE__, ##x)

#define TEST_ERR(format, x...) \
	ALOGE("%s(%d) " format, __func__, __LINE__, ##x)

class TestLog {
public:
    TestLog();
    TestLog(string logpath);
    ~TestLog();
    int print(const char *format, ...);
    void setPath(string logpath);
    typedef enum {
        LSTDIO = 0,
        LALOGE,
        LFILE,
    }LogType;
    LogType             mType;
    FILE*               mFile;
    string              mPath;
    string              mTag;
    bool                mIsNewPath;
};
#endif
