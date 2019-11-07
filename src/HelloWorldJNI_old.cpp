#include "HelloWorldJNI.h"
#include <iostream>

/*
 * Class:     HelloWorldJNI
 * Method:    sayHello
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT void JNICALL Java_HelloWorldJNI_sayHello (JNIEnv* env, jobject thisObject) {
	std::string hello = "Hello from C++ !!";
    std::cout << hello << std::endl;
    //return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jlong JNICALL Java_HelloWorldJNI_sumIntegers(JNIEnv* env, jobject thisObject, jint first, jint second) {
    std::cout << "C++: The numbers received are : " << first << " and " << second << std::endl;
    return (long)first + (long)second;
}

JNIEXPORT jstring JNICALL Java_HelloWorldJNI_sayHelloToMe(JNIEnv* env, jobject thisObject, jstring name, jboolean isFemale) {
    const char* nameCharPointer = env->GetStringUTFChars(name, NULL);
    std::string title;
    if(isFemale) {
        title = "Ms. ";
    }
    else {
        title = "Mr. ";
    }
 
    std::string fullName = title + nameCharPointer;
    return env->NewStringUTF(fullName.c_str());
}

JNIEXPORT jobject JNICALL Java_HelloWorldJNI_createUser
  (JNIEnv *env, jobject thisObject, jstring name, jdouble balance) {
   	// Create the object of the class UserData
    jclass userDataClass = env->FindClass("UserData");
    jobject newUserData = env->AllocObject(userDataClass);
     
    // Get the UserData fields to be set
    jfieldID nameField = env->GetFieldID(userDataClass , "name", "Ljava/lang/String;");
    jfieldID balanceField = env->GetFieldID(userDataClass , "balance", "D");
     
    env->SetObjectField(newUserData, nameField, name);
    env->SetDoubleField(newUserData, balanceField, balance);
     
    return newUserData;
}
 
JNIEXPORT jstring JNICALL Java_HelloWorldJNI_printUserData
  (JNIEnv *env, jobject thisObject, jobject userData) {
     
    // Find the id of the Java method to be called
    jclass userDataClass=env->GetObjectClass(userData);
    jmethodID methodId=env->GetMethodID(userDataClass, "getUserInfo", "()Ljava/lang/String;");
 
    jstring result = (jstring)env->CallObjectMethod(userData, methodId);
    return result;
}
