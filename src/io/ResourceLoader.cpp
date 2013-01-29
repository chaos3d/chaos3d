#include "res/ResourceLoader.h"
#include "res/Resource.h"

//using namespace chaos;

void ResourceLoader::unload(Resource* res){
	delete res;
}