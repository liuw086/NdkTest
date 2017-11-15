/**
 * @file            
 * @description     
 * @author          yuzegao@xiaomi.com
 * @version         1.0
 * @date            2015-06-01
 */


#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <unistd.h>
#include "logger.h"
#include "milive_agent_jni.h"
#include "AgentManager.h"



#ifdef ANDROID
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"MiLiveAgent" ,__VA_ARGS__)
#endif
using namespace milive;
using namespace mip2p;
#define SCORE_TEST 0

#if SCORE_TEST
#include "SchedulingOptimize.h"
using namespace milive;
#endif

static JavaVM* g_JavaVM = NULL;
static jobject g_context = NULL;
static jclass systemInfoClass;
static jclass wifiInfoClass;

static JNINativeMethod gMethods[] = {
    {
        "startAgent",
        "(Landroid/content/Context;)V", //方法的签名
        (void*)startAgent
    },
    {
        "stopAgent",
        "()V", //方法的签名
        (void*)stopAgent
    },
    {
        "getLocalUrlByCdnUrl",
        "(Ljava/lang/String;)Ljava/lang/String;", //方法的签名
        (void*)getLocalUrlByCdnUrl
    },
    {
        "setMaxCacheSize",
        "(J)V", //方法的签名
        (void*)setMaxCacheSize
    },
    {
        "setCacheDiskPath",
        "(Ljava/lang/String;)V", //方法的签名
        (void*)setCacheDiskPath
    }
};


static int registerNativeMethod(JNIEnv *env) {
    
    //由于JNINativeMethod结构体中使用的函数名并非全路径，所以要指明是哪个类
    jclass clazz = env->FindClass("com/xiaomi/agent/MiLiveAgent");
    //调用JNIEnv的RegisterNatives函数，注册关联关系    
    jint result = env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    return result;
}

#if SCORE_TEST


void * run_thread(void * arg)
{
    LOGGER(ERROR, "start thread");
    sleep(1);

    SchedulingOptimize so;
    //so.RecordSsid();

    std::vector<std::pair<std::string, int> > ipPortList;
    ipPortList.push_back(make_pair("125.16.124.70", 1935));
    ipPortList.push_back(make_pair("125.17.240.1", 1935));

    for (int i = 0; i < ipPortList.size(); i++) {
        LOGGER(LOG_NORMAL, "input original hosts:%s:%d", ipPortList[i].first.c_str(), ipPortList[i].second);
    }
    ipPortList = so.SortAddresses(ipPortList);
    for (int i = 0; i < ipPortList.size(); i++) {
        LOGGER(LOG_NORMAL, "output sorted hosts:%s:%d", ipPortList[i].first.c_str(), ipPortList[i].second);
    }


    return NULL;

}

#endif
//jstring charTojstring(JNIEnv* env, const char* pat) {
//    //定义java String类 strClass
//    jclass strClass = env->FindClass("Ljava/lang/String;");
//    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
//    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
//    //建立byte数组
//    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
//    //将char* 转换为byte数组
//    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);
//    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
//    jstring encoding = (env)->NewStringUTF("utf-8");
//    //将byte数组转换为java String,并输出
//    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
//}
jstring charTojstring(JNIEnv* env, const char* pStr)
{
    int        strLen    = strlen(pStr);
    jclass jstrObj = env->FindClass("java/lang/String");
    jmethodID mid = env->GetMethodID(jstrObj, "<init>", "([BLjava/lang/String;)V");
    jbyteArray byteArray =env->NewByteArray(strLen);
    jstring    encode    = (env)->NewStringUTF("utf-8");
    (env)->SetByteArrayRegion(byteArray, 0, strLen, (jbyte*)pStr);

    return (jstring)(env)->NewObject(jstrObj, mid, byteArray, encode);
}

char* jstringToChar(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

void stopAgent(JNIEnv * env, jclass cls){
    AgentManager::instance()->stopAgent();

    LOGE("start call stopAgent ");
    LOGGER(LOG_DEBUG, " start call stopAgent ");
}

jstring getLocalUrlByCdnUrl(JNIEnv * env, jclass cls, const jstring cdnUrl){
    std::string s_cdnUrl = jstringToChar(env, cdnUrl);
    std::string s_localUrl = AgentManager::instance()->getLocalUrlByCdnUrl(s_cdnUrl);

    LOGGER(LOG_DEBUG, " s_cdnUrl=%s ; s_localUrl=%s", s_cdnUrl.c_str(), s_localUrl.c_str());
    return charTojstring(env, s_localUrl.c_str());
}
void setMaxCacheSize(JNIEnv * env, jclass cls, jlong size){
    AgentManager::instance()->setMaxCacheSize(size);
    LOGGER(LOG_DEBUG, " maxcache=%d B", size);
}

void setCacheDiskPath(JNIEnv * env, jclass cls, jstring path){
    std::string s_path = jstringToChar(env, path);
    AgentManager::instance()->setCacheDiskPath(s_path);

    LOGGER(LOG_DEBUG, " path=%s ", s_path.c_str());
}

void startAgent(JNIEnv * env, jclass cls, jobject obj)
{
    struct rlimit lim;
    lim.rlim_cur = RLIM_INFINITY;
    lim.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &lim);
    g_context = env->NewGlobalRef(reinterpret_cast<jobject>(obj));


    jclass localclass = env->FindClass("com/xiaomi/agent/SystemInfo");
    if (localclass == NULL)
    {
        LOGE("find class failed:com/xiaomi/agent/SystemInfo");
        return ;
    }
    systemInfoClass = (jclass)env->NewGlobalRef(localclass);
    if (systemInfoClass == NULL)
    {
        LOGE("NewGlobalRef failed systemInfoClass");
        return ;
    }

    localclass = env->FindClass("com/xiaomi/agent/WifiInfo");
    wifiInfoClass = (jclass)env->NewGlobalRef(localclass);
    if(wifiInfoClass == NULL)
    {
        LOGE("NewGlobalRef failed wifiInfoClass");
        return;
    }
    
    SCOPTED_LOG;//只能放这里，前面还未初始化g_context
    milive::AgentManager::instance()->startAgent(0);

    LOGE("start call startAgent ");
    LOGGER(LOG_DEBUG, " start call startAgent ");

#if SCORE_TEST
    milive::WifiInfo * wifi = milive::UtilsJni::GetCurrentWifiInfo();
    LOGGER(DEBUG , "wifi info:%s ", wifi->ssid.c_str());
    pthread_t thread;
    pthread_create(&thread, NULL, run_thread, NULL);
#endif

    return ;
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    g_JavaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv( (void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);
    if (registerNativeMethod(env) < 0) { return -1; }
    return JNI_VERSION_1_4;//必须返回这个值，否则报错
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{

    JNIEnv *env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return;
    }
    env->DeleteGlobalRef(g_context);
    env->DeleteGlobalRef(systemInfoClass);
    env->DeleteGlobalRef(wifiInfoClass);
}

namespace milive{

    std::string UtilsJni::GetStoragePath()
    {
        std::string defaultDir = "/sdcard";
        JNIEnv *env;
        if (g_JavaVM == NULL || g_context == NULL)
        {
            LOGE("g_javaVm:%p or g_conetext:%p empty", g_JavaVM, g_context);
            return defaultDir;
        }
        AttachThreadScoped ats(g_JavaVM);
        env = ats.env();
        JNILocalRefGuard guard(env);

        jmethodID j_getStoragePath = env->GetStaticMethodID(systemInfoClass, "getStoragePath", "(Landroid/content/Context;)Ljava/lang/String;");
        if (j_getStoragePath == NULL){
            LOGE("find getStoragePath failed");
            return defaultDir;
        }

        jstring isp = (jstring)env->CallStaticObjectMethod(systemInfoClass, j_getStoragePath, g_context);

        const char* cpath = env->GetStringUTFChars(isp, NULL);
        std::string storagePath = cpath;
        env->ReleaseStringUTFChars(isp, cpath);
        if (storagePath == "") {
            storagePath = defaultDir;
        }
        LOGE("storage path:%s", storagePath.c_str());
        return storagePath;
    }

    WifiInfo* UtilsJni::GetCurrentWifiInfo()
    {
        SCOPTED_LOG;
        static WifiInfo data;
        JNIEnv *env = NULL;
        if (g_JavaVM == NULL || g_context == NULL)
        {
            LOGGER(LOG_DEBUG, "g_JavaVM or g_context empty");
            return NULL;
        }

        AttachThreadScoped ats(g_JavaVM);
        env = ats.env();
        JNILocalRefGuard guard(env);

        jmethodID j_getNetworkWifiId = env->GetStaticMethodID(systemInfoClass, "getCurrentWifiInfo", "(Landroid/content/Context;)Lcom/xiaomi/agent/WifiInfo;");
        if (j_getNetworkWifiId == NULL) {
            LOGGER(LOG_DEBUG, "find class getCurrentWifiInfo failed, (Landroid/content/Context;)Lcom/xiaomi/agent/WifiInfo");
            return NULL;
        }

        jobject j_data = env->CallStaticObjectMethod(systemInfoClass, j_getNetworkWifiId, g_context);

        jmethodID j_get_bssid = env->GetMethodID(wifiInfoClass, "getBssid", "()Ljava/lang/String;");
        jmethodID j_get_ssid = env->GetMethodID(wifiInfoClass, "getSsid", "()Ljava/lang/String;");
        jstring jwifiId = (jstring)env->CallObjectMethod(j_data, j_get_bssid, NULL);
        jstring jwifiName = (jstring)env->CallObjectMethod(j_data, j_get_ssid, NULL);

        const char* wifiIdCstr = env->GetStringUTFChars(jwifiId, NULL);
        const char* wifiNameCstr = env->GetStringUTFChars(jwifiName, NULL);
        std::string wifiId = wifiIdCstr;
        std::string wifiName = wifiNameCstr;
        data.ssid = wifiName;
        data.bssid = wifiId;
        LOGGER(LOG_DEBUG, "get ssid:%s, bssid:%s", wifiName.c_str(), wifiId.c_str());
        LOGE( "get ssid:%s, bssid:%s", wifiName.c_str(), wifiId.c_str());

        env->ReleaseStringUTFChars(jwifiId, wifiIdCstr);
        env->ReleaseStringUTFChars(jwifiName, wifiNameCstr);
        return &data;

    }


    AttachThreadScoped::AttachThreadScoped(JavaVM* jvm) : attached_(false), jvm_(jvm), env_(NULL)
    {
        jint ret_val = jvm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_4);
        if (ret_val == JNI_EDETACHED) {
            // Attach the thread to the Java VM.
            ret_val = jvm_->AttachCurrentThread(&env_, NULL);
            attached_ = ret_val == JNI_OK;
            assert(attached_);
        }
    }

    AttachThreadScoped::~AttachThreadScoped()
    {
        if (attached_ && (jvm_->DetachCurrentThread() < 0)) {
            assert(false);
        }
    }

    JNIEnv* AttachThreadScoped::env()
    {
        return env_;
    }


    JNILocalRefGuard::JNILocalRefGuard(JNIEnv* env, int reservedCapacity) :env_(env) {
        int ret = env_->PushLocalFrame(reservedCapacity);
        if (ret != 0) {
            
        }
    }

    JNILocalRefGuard::~JNILocalRefGuard() {
        env_->PopLocalFrame(NULL);
    }

    void JNILocalRefGuard::dumpLocalRefTable() {
        jclass vm_class = env_->FindClass("dalvik/system/VMDebug");
        jmethodID dump_mid = env_->GetStaticMethodID(vm_class, "dumpReferenceTables", "()V");
        env_->CallStaticVoidMethod(vm_class, dump_mid);
        env_->DeleteLocalRef(vm_class);
    }

}

