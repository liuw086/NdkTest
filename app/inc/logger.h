/**
 * @file            log.h
 * @brief           日志类
 * @author          yuzegao@xiaomi.com
 * @version         1.0
 * @date            2016-03-15
 */

#ifndef _LOGGER_H__
#define _LOGGER_H__

#include "tlog.h"
#include <string>

using namespace mip2p;

#ifdef ANDROID
//#include "jni/milive_transport_jni.h"

#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"MiLiveTransport" ,__VA_ARGS__)
#endif

#ifndef uint32
#define uint32 uint32_t
#endif

namespace mip2p {

//const int LOG_DEBUG = 0;
//const int LOG_INFO = 1;
//const int LOG_WARNING = 2;
//const int LOG_ERROR = 3;
//const int LOG_FATAL = 4;

//
//#ifdef DEBUG
//#undef DEBUG
//#endif
//#define DEBUG   MSTORE_LOG_DEBUG
//
//#ifdef INFO
//#undef INFO
//#endif
//#define INFO    MSTORE_LOG_INFO
//
//#ifdef WARNING
//#undef WARNING
//#endif
//#define WARNING MSTORE_LOG_WARNING
//
//#ifdef ERROR
//#undef ERROR
//#endif
//#define ERROR   MSTORE_LOG_ERROR
//
//#ifdef FATAL
//#undef FATAL
//#endif
//#define FATAL   MSTORE_LOG_FATAL


class MSLoggerPath {

public:
    static MSLoggerPath* Instance() {
        static MSLoggerPath stance("");
        return &stance;
    }

    std::string getfpath(){
        return fpath;
    }

    void setfpath(std::string path){
        fpath = path;
    }

private:
    MSLoggerPath( const char* filePath ) {
    }
private:
    std::string fpath;

    MSLoggerPath() {

    }
    ;
    ~MSLoggerPath() {
    }
    ;
};


class MSLogger {

public:
    static MSLogger * Instance(void) {
        if (ptr == NULL) {
            ptr = new MSLogger("");
        }
        return ptr;
    }
    void SetTLog(CTLog * ptr) {
        log = ptr;
    }
    CTLog* GetTLog(void) {
        return log;
    }
private:
    MSLogger(std::string str) {
        CTLog* tLog = new CTLog;
#ifdef ANDROID
        std::string path ="/sdcard";//= UtilJni::GetStoragePath();
        path += "/voip-data/com.wali.live/";
        //LOGE("path dir:%s", path.c_str());
        if (tLog->log_open(LOG_TRACE, LOG_TYPE_DAILY, path.c_str(), "trace_network", 8*1024000, 2) != 0) {
            //LOGE("create log file faied");
            printf("initial log failed\n");
        }
#else
        std::string path = MSLoggerPath::Instance()->getfpath();

    if (tLog->log_open(LOG_TRACE, LOG_TYPE_DAILY, path.c_str(), "trace_network", 8*1024*1024, 2) != 0) {
        printf("initial log failed\n");
    }
#endif
        log = tLog;
    }
    ;
    ~MSLogger() {
        delete log;
    }
    ;

    static MSLogger * ptr;

    CTLog* log;


};

//MSLogger * MSLogger::ptr = NULL;

#define LOGGER(severity, fmt, args...)  \
  do { \
    switch (severity) {  \
      case 0:  \
         MSLogger::Instance()->GetTLog()->LOG_P_ALL( LOG_TRACE, fmt"\n", ##args); \
         break;\
      case 1:  \
         MSLogger::Instance()->GetTLog()->LOG_P_ALL( LOG_DEBUG, fmt"\n", ##args); \
         break; \
      case 2:  \
         MSLogger::Instance()->GetTLog()->LOG_P_ALL( LOG_NORMAL, fmt"\n", ##args); \
         break; \
      case 3: \
         MSLogger::Instance()->GetTLog()->LOG_P_ALL( LOG_ERROR, fmt"\n", ##args); \
         break; \
      case 4: \
         MSLogger::Instance()->GetTLog()->LOG_P_ALL( LOG_FATAL, fmt"\n", ##args); \
        break;  \
      default: \
         MSLogger::Instance()->GetTLog()->LOG_P_ALL( LOG_ERROR, fmt"\n", ##args); \
         break; \
    } \
  } while (0)

class ScopedLog {
public:
    ScopedLog(const char* fun, const char * file, unsigned int line) :
            mName(fun), fName(file), line(line) {
        LOGGER(LOG_DEBUG, "CALLTRACE [%s:%d] %s ENTER", file, line, fun);
    }
    ~ScopedLog() {
        LOGGER(LOG_DEBUG, "CALLTRACE [%s:%d] %s LEAVE", fName, line, mName);
    }

private:
    const char* mName;
    const char* fName;
    unsigned int line;
};

#define SCOPTED_LOG_IMP(X, Y, Z) ScopedLog log(X, Y, Z)
#define SCOPTED_LOG SCOPTED_LOG_IMP(__FUNCTION__, __FILE__, __LINE__)

// disable trance log, uncomment below line
//#define SCOPTED_LOG
}
#endif


