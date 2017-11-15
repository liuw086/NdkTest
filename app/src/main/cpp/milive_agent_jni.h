/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_mi_dennis_jnitest_JniTest */
#include <string>
#include <vector>


#ifndef _Included_com_mi_agent_jnitest_JniTest
#define _Included_com_mi_agent_jnitest_JniTest
#ifdef __cplusplus
extern "C" {
#endif

    void startAgent(JNIEnv * env, jclass cls, jobject obj);

    void stopAgent(JNIEnv * env, jclass cls);
    jstring getLocalUrlByCdnUrl(JNIEnv * env, jclass cls, const jstring cdnUrl);
    void setMaxCacheSize(JNIEnv * env, jclass cls, jlong size);
    void setCacheDiskPath(JNIEnv * env, jclass cls, jstring path);

    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);
    JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);

#ifdef __cplusplus
}
#endif


namespace milive
{

    class AttachThreadScoped
    {
    public:
        explicit AttachThreadScoped(JavaVM* jvm);
        ~AttachThreadScoped();
        JNIEnv* env();

    private:
        bool attached_;
        JavaVM* jvm_;
        JNIEnv* env_;
    };


    class JNILocalRefGuard
    {
    public:
        JNILocalRefGuard(JNIEnv* env, int reservedCapacity = 16);
        ~JNILocalRefGuard();
        void dumpLocalRefTable();
    private:
        JNIEnv *env_;
    };


    class WifiInfo {
    public:
        std::string bssid;
        std::string ssid;
    };

    class UtilsJni{
    public:
        UtilsJni();
        virtual ~UtilsJni();
        static void RegisterUtilJVM(void* jvm, void* env, void* context);
        static void UnRegisterUtilJVM();
    public:
        static std::string GetStoragePath();
        static WifiInfo* GetCurrentWifiInfo();
        static std::vector<WifiInfo> GetAvailableWifiInfo();
    private:


    };
}//namespace xplatform_util



#endif