#include <jni.h>
#include <string>
#include <iostream>

#include "AgentManager.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_liuwei_ndktest_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";


    std::cout<<"test"<<std::endl;

    return env->NewStringUTF(hello.c_str());
}
