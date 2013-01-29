#include "IOManager.h"
#include "StreamLocator.h"

#import <Foundation/Foundation.h>

//using namespace chaos;
using namespace std;

void IOManager::registerDefaults(){
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                         NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
	
	string base_loc[2] = {"app","doc"};
	NSString* base[2] = {
		[[[NSBundle mainBundle] bundlePath] stringByAppendingString: @"/"],
		[documentsDirectory stringByAppendingString: @"/"],		
	};
	
	mApplicationPath = [base[0] UTF8String];
	mDocPath = [base[1] UTF8String];
	// automatically add the .pkg file into locators	
	for( int i=0;i<2; ++i ){
		mFileLoc.insert( pair<string,StreamLocator*>( base_loc[i], 
													 new io::DirLocator( [base[i] UTF8String] ) ));
		
		NSDirectoryEnumerator *dirEnum = [[NSFileManager defaultManager] enumeratorAtPath: base[i]];
		NSString *file = 0;
		[dirEnum skipDescendents];
		while( file = [dirEnum nextObject] ){
			if( [[file pathExtension] isEqualToString: @"pkg"] ){
				mFileLoc.insert( pair<string,StreamLocator*>( base_loc[i], 
															 new io::PkgFileLocator( [[base[i] stringByAppendingPathComponent: file] UTF8String]) ));
			}
		}
	}
	[pool release];
}

