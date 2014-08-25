#!/usr/bin/env bash

VERSION=5.1.5

if [ -n "$2" ]; then
    VERSION="$2"
fi

rm lua.tar.gz
curl -L http://www.lua.org/ftp/lua-$VERSION.tar.gz > lua.tar.gz

if [ ! -f lua.tar.gz ]; then
   >&2 echo error downloading lua;
   exit 1;
fi

rm -rf lua-$VERSION
tar xvf lua.tar.gz
cd lua-$VERSION

make CC="xcrun -sdk iphoneos clang -arch armv7" CFLAGS="-Ofast -DLUA_USE_POSIX" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-armv7 install

make CC="xcrun -sdk iphoneos clang -arch armv7s" CFLAGS="-Ofast -DLUA_USE_POSIX" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-armv7s install

make CC="xcrun -sdk iphoneos clang -arch arm64" CFLAGS="-Ofast -DLUA_USE_POSIX" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-arm64 install

make CC="xcrun -sdk iphonesimulator clang -arch i386 -mios-version-min=5.0" CFLAGS="-Ofast -DLUA_USE_POSIX" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-i386 install

make CC="xcrun -sdk iphonesimulator clang -arch x86_64 -mios-version-min=5.0" CFLAGS="-Ofast -DLUA_USE_POSIX" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-x86_64 install

mkdir -p output-ios output-macosx output-include
lipo -create $(find install-*/lib -name "*.a") -o output-ios/liblua.$VERSION.a
cp install-x86_64/include/*.h output-include/.

make CC="xcrun -sdk macosx clang -arch x86_64 -mmacosx-version-min=10.7" CFLAGS="-Ofast -DLUA_USE_MACOSX" MYLIBS="-lreadline" MYLDFLAGS="-flto" clean generic
make INSTALL_TOP=`pwd`/install-macosx install
cp install-macosx/lib/liblua.a output-macosx/liblua.$VERSION.a

mkdir -p ../liblua/$VERSION/
if [ "$1" == "link" ]; then
    (cd output-ios && ln -s liblua.$VERSION.a liblua.a)
    (cd output-macosx && ln -s liblua.$VERSION.a liblua.a)
    (cd ../liblua/ && ln -sF $VERSION lua)
fi

cp output-ios/*.a ../libs/ios/.
cp output-macosx/*.a ../libs/macosx/.
cp output-include/*.h ../liblua/$VERSION/

cd ..
