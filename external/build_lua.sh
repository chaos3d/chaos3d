#!/usr/bin/env bash

: ${DEV_DIR:=`xcode-select -p`}
: ${VERSION:=5.1.5}

rm lua.tar.gz
curl -L http://www.lua.org/ftp/lua-$VERSION.tar.gz > lua.tar.gz

if [ ! -f lua.tar.gz ]; then
   >&2 echo error downloading lua;
   exit 1;
fi

rm -rf lua-$VERSION
tar xvf lua.tar.gz
cd lua-$VERSION

if [ $VERSION = "5.1.5" ]; then
    patch -p0 <<EOF
--- src/loadlib.c
+++ src/loadlib.c
@@ -61,14 +61,11 @@
 #include <dlfcn.h>
 
 static void ll_unloadlib (void *lib) {
-  dlclose(lib);
 }
 
 
 static void *ll_load (lua_State *L, const char *path) {
-  void *lib = dlopen(path, RTLD_NOW);
-  if (lib == NULL) lua_pushstring(L, dlerror());
-  return lib;
+  return RTLD_MAIN_ONLY;
 }
EOF
elif [ $VERSION = "5.2.3" ]; then
    patch -p0 <<EOF
--- src/loadlib.c
+++ src/loadlib.c
@@ -61,14 +61,11 @@
 #include <dlfcn.h>
 
 static void ll_unloadlib (void *lib) {
-  dlclose(lib);
 }
 
 
 static void *ll_load (lua_State *L, const char *path, int seeglb) {
-  void *lib = dlopen(path, RTLD_NOW | (seeglb ? RTLD_GLOBAL : RTLD_LOCAL));
-  if (lib == NULL) lua_pushstring(L, dlerror());
-  return lib;
+  return RTLD_MAIN_ONLY;
 }
EOF
fi

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk iphoneos clang -arch armv7" CFLAGS="-Ofast -DLUA_USE_POSIX -DLUA_DL_DLOPEN" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-armv7 install

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk iphoneos clang -arch armv7s" CFLAGS="-Ofast -DLUA_USE_POSIX -DLUA_DL_DLOPEN" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-armv7s install

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk iphoneos clang -arch arm64" CFLAGS="-Ofast -DLUA_USE_POSIX -DLUA_DL_DLOPEN" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-arm64 install

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk iphonesimulator clang -arch i386 -mios-version-min=5.0" CFLAGS="-Ofast -DLUA_USE_POSIX -DLUA_DL_DLOPEN" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-i386 install

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk iphonesimulator clang -arch x86_64 -mios-version-min=5.0" CFLAGS="-Ofast -DLUA_USE_POSIX -DLUA_DL_DLOPEN" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-x86_64 install

mkdir -p output-ios output-macosx output-include
lipo -create $(find install-*/lib -name "*.a") -o output-ios/liblua.$VERSION.a
cp install-arm64/include/* output-include/.

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk macosx clang -arch x86_64 -mmacosx-version-min=10.7" CFLAGS="-Ofast -DLUA_USE_LINUX" MYLIBS="-lreadline" MYLDFLAGS="-flto" \
    LUA_A="liblua.dylib" AR="xcrun -sdk macosx clang -arch x86_64 -dynamiclib -o" RANLIB="strip -u -r" clean generic
make TO_LIB="liblua.dylib" INSTALL_TOP=`pwd`/install-macosx64 install

make CC="env DEVELOPER_DIR=$DEV_DIR xcrun -sdk macosx clang -arch i386 -mmacosx-version-min=10.7" CFLAGS="-Ofast -DLUA_USE_LINUX" MYLIBS="-lreadline" MYLDFLAGS="-flto" \
    LUA_A="liblua.dylib" AR="xcrun -sdk macosx clang -arch i386 -dynamiclib -o" RANLIB="strip -u -r" clean generic
make TO_LIB="liblua.dylib" INSTALL_TOP=`pwd`/install-macosx32 install

lipo -create $(find install-macosx*/lib -name "*.dylib") -o output-macosx/liblua.$VERSION.dylib
install_name_tool -id @executable_path/liblua.dylib output-macosx/liblua.$VERSION.dylib

lipo -create $(find install-macosx*/bin/ -name "lua") -o output-macosx/lua
lipo -create $(find install-macosx*/bin/ -name "luac") -o output-macosx/luac
install_name_tool -change liblua.dylib @executable_path/liblua.dylib output-macosx/lua
install_name_tool -change liblua.dylib @executable_path/liblua.dylib output-macosx/luac

mkdir -p ../liblua/$VERSION/
if [ "$1" == "link" ]; then
    (cd output-ios && ln -s liblua.$VERSION.a liblua.a)
    (cd output-macosx && ln -sF liblua.$VERSION.dylib liblua.dylib)
    (cd ../liblua/ && ln -sF $VERSION lua)
fi

cp output-ios/*.a ../libs/ios/.
cp output-macosx/* ../libs/macosx/.
cp output-include/*.h output-include/*.hpp ../liblua/$VERSION/

cd ..
