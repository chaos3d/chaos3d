#!/usr/bin/env bash

curl -L https://github.com/wxWidgets/wxWidgets/archive/WX_3_0_1.zip > wx.zip

if ! test -f wx.zip; then
    >&2 echo error downloading wx;
    exit 1;
fi;

rm -rf wx
unzip wx.zip -d wx
CURDIR=`pwd`

rm -rf wxsys
mkdir -p wxsys/include
(cd wxsys && ln -sF ../libs/macosx lib)

# symlink to system-alike
(cd wxsys/lib/ && ln -sF libpng17.dylib libpng.dylib)
cp libpng/* wxsys/include/.

cd wx/$(ls wx)
./configure --prefix=`pwd`/install CC="xcrun clang -I$CURDIR/wxsys/include/" CXX="xcrun clang++ -I$CURDIR/wxsys/include" \
    CXXCPP="xcrun clang++ -E -std=c++11"  \
    CFLAGS="-Ofast" CXXFLAGS="-Ofast -stdlib=libc++ -std=c++11" LDFLAGS="-stdlib=libc++ -flto -L$CURDIR/wxsys/lib" \
    CPP="clang -E" OBJCXXFLAGS="-stdlib=libc++ -std=c++11" \
    --with-macosx-sdk=`xcrun --show-sdk-path`  --enable-stl --enable-unicode --with-osx_cocoa  --with-libjpeg=builtin \
    --with-libpng=sys --with-libtiff=no --disable-richtext --with-cocoa -with-macosx-version-min=10.7 --enable-compat28

make -j8 && make install || (>&2 echo error building wx; exit 1)
./change-install-names install/lib/ install/bin/ "@rpath" `pwd`/install/lib

mkdir -p $CURDIR/libs/wx $CURDIR/libwx
cp -a install/lib/*.dylib $CURDIR/libs/wx/.
cp -a install/include/* $CURDIR/libwx/.
install/bin/wx-config --cxxflags | sed -e "s:`pwd`/install/::g" -e "s:lib/:libs/wx/:"> $CURDIR/libs/wx/config
cd $CURDIR

# remove the hack
rm wxsys/lib/libpng.dylib
