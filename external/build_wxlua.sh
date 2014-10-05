#!/usr/bin/env bash

URL=https://github.com/wuhao5/wxlua/archive/v3.0-pre.tar.gz
curl -L "$URL" > wxlua.tar.gz

if ! test -f wxlua.tar.gz; then
    >&2 echo error downloading wxlua;
    exit 1;
fi;

rm -rf wxlua-src
(mkdir wxlua-src && tar xvf wxlua.tar.gz -C wxlua-src)

CURDIR=`pwd`
WXDIR=`pwd`/wx/$(ls wx)/
LUADIR=`pwd`/liblua/5.1.5/
LUALIB=`pwd`/libs/macosx/liblua.dylib

if ! test -d $LUADIR && test -f $LUALIB; then 
    >&2 echo Lua does not exist
    exit -1;
elif ! test -d $WXDIR; then
    >&2 echo wx does not exist
    exit -1;
fi

cd wxlua-src/$(ls wxlua-src)/wxLua

mkdir macosx && cd macosx
cmake .. -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=`pwd`/install -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DCMAKE_C_COMPILER=`xcrun -f clang` -DCMAKE_CXX_COMPILER=`xcrun -f clang++` \
    -DwxWidgets_CONFIG_EXECUTABLE=$WXDIR/install/bin/wx-config  -DwxWidgets_COMPONENTS="stc;html;aui;adv;core;net;base;webview;xrc;xml;media" \
    -DwxLuaBind_COMPONENTS="stc;html;aui;adv;core;net;base;webview;xrc;xml;media" -DwxLua_LUA_LIBRARY_USE_BUILTIN=FALSE \
    -DCMAKE_CXX_FLAGS='-std=c++11 -stdlib=libc++ -Wno-c++11-narrowing' \
    -DwxLua_LUA_LIBRARY_BUILD_SHARED=TRUE -DwxLua_LUA_INCLUDE_DIR=$LUADIR -DwxLua_LUA_LIBRARY=$LUALIB

make && make install || (>&2 echo error building wxlua module && exit -1;)

mkdir -p $CURDIR/wxlua/.
cp install/lib/libwx.dylib $CURDIR/libs/macosx/.
cp -r install/include/wxlua/* $CURDIR/wxlua/.
#install_name_tool -id @executable_path/libwx.dylib $CURDIR/libs/macosx/libwx.dylib
otool -l $CURDIR/libs/macosx/libwx.dylib | grep -A 4 RPATH | egrep "path.*" -o | cut -d ' ' -f 2 | \
    while read line; do
        install_name_tool -delete_rpath $line $CURDIR/libs/macosx/libwx.dylib
    done;

install_name_tool -add_rpath @loader_path/ $CURDIR/libs/macosx/libwx.dylib

cd $CURDIR
