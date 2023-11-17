/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include "bridge.h"
#include "client.h"

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
extern "C"
{
	const char* convertToUTF(JNIEnv* env, jstring str) {
        const char *convertedValue = (char*)env->GetStringUTFChars(str, 0);
		return convertedValue;
	}
	JNIEXPORT jstring JNICALL Java_org_aaa_rakclient_HelloJni_stringFromJNI(JNIEnv* env, jobject thiz)
	{
		return env->NewStringUTF("Hello from JNI !");
	}
	
	JNIEXPORT void JNICALL Java_org_aaa_rakclient_HelloJni_nativeInit(JNIEnv* env, jobject idk) {
		Bridge::getInstance().bridge_init(env, idk);
		//bridge needs to be initialized first!
		//not anymore, client has to init bridge
		//woops, turns out if we wanna add usernames, we gotta set the client's username handler..
	}
	
	JNIEXPORT void JNICALL Java_org_aaa_rakclient_HelloJni_sendMessageNative(JNIEnv* env, jobject idk, jstring msg) {
		char* convertedValue = (char*)convertToUTF(env, msg);
		Client::getInstance().message(convertedValue);
		env->ReleaseStringUTFChars(msg, convertedValue);
	}
	
	JNIEXPORT void JNICALL Java_org_aaa_rakclient_HelloJni_setClientUsernameNative(JNIEnv* env, jobject iforgot, jstring name) {
		//const char* convertedValue = convertToUTF(env, name);
		
		Client::getInstance().setUsername(NULL);
		//env->ReleaseStringUTFChars(name, convertedValue);
	}
	
	JNIEXPORT void JNICALL Java_org_aaa_rakclient_HelloJni_startConnectionNative(JNIEnv* env, jobject testing) {
		Client::getInstance().init();
	}
	
	JNIEXPORT void JNICALL Java_org_aaa_rakclient_HelloJni_stopConnectionNative(JNIEnv* env, jobject testing) {
		Client::getInstance().shutdown();
		
		//Client::getInstance() = Client();
	}
}
