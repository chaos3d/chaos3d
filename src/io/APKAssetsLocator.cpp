#include "io/APKAssetsLocator.h"
#include "io/JavaInputStream.h"
#include "platform/jni/jni.h"

using namespace std;
using namespace chaos;
using namespace chaos::io;

APKAssetsLocator::~APKAssetsLocator(){
}

APKAssetsLocator::APKAssetsLocator(const char* dir) : mBase(dir){
	typedef JavaInputStream::FileList List;
	List files = JavaInputStream::list(mBase.c_str());

	for(List::iterator it = files.begin(); it != files.end(); ++it){
		LOG("adding file... (%s)", it->c_str());
		mHash.insert(*it);
	}
}

bool APKAssetsLocator::exist(char const *path){
	typedef TFileHash::iterator iterator;
	typedef pair<iterator,bool> result;

	LOG("finding... (%s)", path);
	return mHash.find(string(path)) != mHash.end();
}

DataStream* APKAssetsLocator::stream(const char *path){
	LOG("open stream... (%s)", path);
	return JavaInputStream::open(path);
}

std::string	APKAssetsLocator::url(const char* path){
	return mBase + path;
}
