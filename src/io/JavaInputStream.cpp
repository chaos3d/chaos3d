/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/JavaInputStream.h"
#include "core/core.h"

using namespace chaos;
using namespace chaos::jni;

jclass		JavaInputStream::JByteBuffer;
jmethodID	JavaInputStream::midRead, JavaInputStream::midPosition, JavaInputStream::midSeek, JavaInputStream::midSize;
jmethodID	JavaInputStream::midCreateIS, JavaInputStream::midGetChannel, JavaInputStream::midWrap;
jmethodID	JavaInputStream::midOpenFD, JavaInputStream::midList, JavaInputStream::midStartOff;

JavaInputStream* JavaInputStream::open(char const* filename){
	return (JavaInputStream*) (new JavaInputStream(filename))->autorelease();
}

JavaInputStream::FileList JavaInputStream::list(char const* dir){
	FileList flist;
	JNIEnvWrapper env;
	initMID(env);

	jstring jdir = env->NewStringUTF(dir);
	jobjectArray files = (jobjectArray)env->CallObjectMethod(jni_assetMgr,midList,jdir);
	jsize js = env->GetArrayLength(files);
	for(jsize i=0;i<js;++i){
		jstring jfile = (jstring)env->GetObjectArrayElement(files,i);
		char const* str = env->GetStringUTFChars(jfile, NULL);
		if (str != 0) {
			flist.push_back(std::string(str));
			env->ReleaseStringUTFChars(jfile, str);
		}else
			break;	// out of memory
		env->DeleteLocalRef(jfile);
	}
	return flist;
}

void JavaInputStream::initMID(JNIEnv *env){
	if( midRead != 0 )	// already inited
		return;

	JByteBuffer = env->FindClass("java/nio/ByteBuffer");
	JByteBuffer = (jclass)env->NewGlobalRef(JByteBuffer);
	midWrap = env->GetStaticMethodID(JByteBuffer, "wrap", "([B)Ljava/nio/ByteBuffer;");

	jclass JFileChannel = env->FindClass("java/nio/channels/FileChannel");
	midRead = env->GetMethodID(JFileChannel, "read", "(Ljava/nio/ByteBuffer;J)I");
	midPosition = env->GetMethodID(JFileChannel, "position", "()J");
	midSeek = env->GetMethodID(JFileChannel, "position", "(J)Ljava/nio/channels/FileChannel;");

	jclass JFileInputStream = env->FindClass("java/io/FileInputStream");
	midGetChannel = env->GetMethodID(JFileInputStream, "getChannel", "()Ljava/nio/channels/FileChannel;");

	jclass JAssetFile = env->FindClass("android/content/res/AssetFileDescriptor");
	midCreateIS = env->GetMethodID(JAssetFile, "createInputStream", "()Ljava/io/FileInputStream;");
	midStartOff = env->GetMethodID(JAssetFile, "getStartOffset", "()J");
	midSize = env->GetMethodID(JAssetFile, "getLength", "()J");

	jclass JAssetManager = env->FindClass("android/content/res/AssetManager");
	midList = env->GetMethodID(JAssetManager, "list", "(Ljava/lang/String;)[Ljava/lang/String;");
	midOpenFD = env->GetMethodID(JAssetManager, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");

}

JavaInputStream::JavaInputStream(char const* filename) : mName(filename){
	create(filename);
}

bool JavaInputStream::create(char const* filename){
	JNIEnvWrapper env;
	initMID(env);

	jstring jfn = env->NewStringUTF(filename);		// String(filename)
	jobject input = env->CallObjectMethod(jni_assetMgr,midOpenFD,jfn);	// input = assetMgr.openFd(filename)
	mStartOff = (size_t)env->CallLongMethod(input, midStartOff);		// input.getStartOffset();
	mSize = (size_t)env->CallLongMethod(input, midSize);

	jobject fis = env->CallObjectMethod(input,midCreateIS);
	mInput = env->CallObjectMethod(fis,midGetChannel);					// FileChannel fc = fd.createInputStream().getChannel();
	mInput = env->NewGlobalRef(mInput);
	mOffset = 0;

	env->CallObjectMethod(mInput, midSeek, (jlong)mStartOff);
	LOG("file:%s, offset:%d size: %d", filename, (int)mStartOff, (int)mSize);
	return true;
}

JavaInputStream::~JavaInputStream(){
	JNIEnvWrapper env;
	env->DeleteGlobalRef(mInput);
}

bool JavaInputStream::end( ){
	return mOffset >= mSize;
}

bool JavaInputStream::valid( ){
	return mInput != 0;
}

size_t JavaInputStream::read(void* buf, size_t bufSize){
	JNIEnvWrapper jenv;
	if( bufSize + mOffset > mSize )
		bufSize = mSize - mOffset;

	jbyteArray jbuf = jenv->NewByteArray(bufSize);		// jbuf = new Byte[size];
	jobject jbb = jenv->CallStaticObjectMethod(JByteBuffer, midWrap, jbuf);	// jbb = ByteBuffer.wrap(jbug);
	jint bufs = jenv->CallIntMethod(mInput, midRead, jbb, (jlong)(mStartOff + mOffset));	// bufs = mInput.read(jbb,mOffset);
	jenv->GetByteArrayRegion(jbuf,0,bufs,(jbyte*)buf);	// buf = jbuf;
	mOffset += bufs;
	jenv->DeleteLocalRef(jbuf);
	jenv->DeleteLocalRef(jbb);
	return (size_t)bufs;
}

bool JavaInputStream::seek(long offset, int pos ){
	if( pos == Seek_Cur ){
		if( mOffset + offset < (long)mSize )
			return mOffset+=offset, true;
		else
			return false;
	}else if( pos == Seek_Set ){
		if( offset < (long)mSize )
			return mOffset=offset, true;
		else
			return false;
	}else if( offset < 0 && -offset < (long)mSize ){
		return mOffset=mSize+offset, true;
	}else
		return false;
	return false;
}

long JavaInputStream::tell(){
	return (long)mOffset;
}

void JavaInputStream::close(){
}
