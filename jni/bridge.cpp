#include "bridge.h"
#include <android/log.h>

const char* convertToUTF(JNIEnv* env, jstring str) {
        const char *convertedValue = (char*)env->GetStringUTFChars(str, 0);
		return convertedValue;
}

const char* convertToUTF(JNIEnv* env, jobject str) {
        const char *convertedValue = (char*)env->GetStringUTFChars((jstring)str, 0);
		return convertedValue;
}

void Bridge::callFunc(jmethodID id, const char* str) {
	__android_log_print(3, "RakClient::Bridge::callFunc", "%s", str);
    env->CallObjectMethod(this->thiz, id, this->env->NewStringUTF(str));
}

jobject Bridge::callFFunc(jmethodID id) {
	__android_log_print(3, "RakClient::Bridge::callFFunc", "%s", "Called this");
    return env->CallObjectMethod(this->thiz, id);
}

void Bridge::callVFunc(jmethodID id, const char* str) {
	__android_log_print(3, "RakClient::Bridge::callVFunc", "%s", str);
    env->CallVoidMethod(this->thiz, id, this->env->NewStringUTF(str));
}

void Bridge::callSFunc(jmethodID id) {
	__android_log_print(3, "RakClient::Bridge::callSFunc", "%s", " ");
    env->CallVoidMethod(this->thiz, id);
}

void Bridge::bridge_init(JNIEnv* newEnv, jobject helloJni) {
	this->env = newEnv;
	this->env->GetJavaVM(&this->jvm);
	this->clazz = reinterpret_cast<jclass>(this->env->NewGlobalRef(this->env->FindClass("org/aaa/rakclient/HelloJni")));
	this->thiz = this->env->NewGlobalRef(helloJni);
    // Get the method that you want to call
    this->getInput = this->env->GetMethodID(this->clazz, "inputHandler", "()Ljava/lang/String;");
	this->lostConn = this->env->GetMethodID(this->clazz, "lostConnection", "()V");
    this->warn = this->env->GetMethodID(this->clazz, "warn", "(Ljava/lang/String;)V");
	this->tell = this->env->GetMethodID(this->clazz, "tell", "(Ljava/lang/String;)V");
	this->ingID = this->env->GetMethodID(this->clazz, "getTheFuckingId", "()Ljava/lang/String;");
	
	//test
	this->callVFunc(this->tell, "Welcome!");
	this->callVFunc(this->tell, "Your device id is: ");
	this->callVFunc(this->tell, convertToUTF(this->env, this->callFFunc(this->ingID)));
}

void Bridge::bridge_warn(const char* what) {
	this->callVFunc(this->warn, what);
}

const char* Bridge::bridge_getDeviceID() {
	return convertToUTF(this->env, this->callFFunc(this->ingID));
}

void Bridge::bridge_tell(const char* what) {
	this->callVFunc(this->tell, what);
}
