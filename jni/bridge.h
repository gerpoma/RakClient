#ifndef __BRIDGE_H
#define __BRIDGE_H
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <string>

class Bridge {
public:
    Bridge() {}
    static Bridge& getInstance()
	{
        static Bridge    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return instance;
    }
jmethodID getInput;
jmethodID lostConn;
jmethodID warn;
jmethodID tell;
jmethodID ingID;
JNIEnv* env;
JavaVM* jvm;
jclass clazz;
jobject thiz;

void callFunc(jmethodID id, const char* str);

void callVFunc(jmethodID id, const char* str);

void callSFunc(jmethodID id);

void bridge_init(JNIEnv* newEnv, jobject helloJni);

void bridge_warn(const char* what);

const char* bridge_getDeviceID();

jobject callFFunc(jmethodID id);

void bridge_tell(const char* what);
};

#endif
